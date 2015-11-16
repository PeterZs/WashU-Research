#include "placeScan_placeScan.h"

#include "placeScan_confidence.h"

#include <iostream>
#include <fstream>
#include <math.h>
#include <time.h>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/eigen.hpp>


static constexpr float fpScale = 86.0*3.28084;  /*pixels per meter */
/*Something like 75 is looking more likely however...Maybe the scanner
isn't in meters?*/
static constexpr double PI = 3.14159265;
static int levelNum = 10;


std::vector<Eigen::Vector2d> zeroZero;
std::vector<int> globalMins;
cv::Mat fpColor;
std::vector<Eigen::Vector3i> truePlacement;



int main(int argc, char *argv[])
{
	gflags::ParseCommandLineFlags(&argc, &argv, true);

	if(FLAGS_debugMode) {
		FLAGS_save = false;
		FLAGS_replace = true;
	}

	cv::Mat floorPlan = cv::imread(FLAGS_floorPlan, 0);
	if(!floorPlan.data) {
		std::cout << "Error reading floorPlan" << std::endl;
		exit(1);
	}

	fpColor = cv::Mat (floorPlan.rows, floorPlan.cols, CV_8UC3, cv::Scalar::all(255));
	for (int i = 0; i < fpColor.rows; ++i) {
		uchar * dst = fpColor.ptr<uchar>(i);
		const uchar * src = floorPlan.ptr<uchar>(i);
		for (int j = 0; j < fpColor.cols; ++j) {
			if(src[j]!=255) {
				dst[j*3] = 128;
				dst[j*3+1] = 128;
				dst[j*3+2] = 128;
			}
		}
	}

	std::vector<Eigen::SparseMatrix<double> > fpPyramid, erodedFpPyramid;
	if(FLAGS_replace)
		place::createFPPyramids(floorPlan, fpPyramid,  erodedFpPyramid);

	std::vector<std::string> pointFileNames;
	std::vector<std::string> rotationFileNames;
	std::vector<std::string> zerosFileNames;

	place::parseFolders(pointFileNames, rotationFileNames, zerosFileNames);
	
	if(FLAGS_numScans == -1 )
		FLAGS_numScans = pointFileNames.size() - FLAGS_startIndex;
	if(FLAGS_stopNumber == -1)
		FLAGS_stopNumber = pointFileNames.size();

	

	
	for(int i = FLAGS_startIndex; i< FLAGS_startIndex + FLAGS_numScans && i < FLAGS_stopNumber; ++i) {
		const std::string scanName = FLAGS_dmFolder + pointFileNames[i];
		const std::string rotationFile = FLAGS_rotFolder + rotationFileNames[i];
		const std::string zerosFile = FLAGS_zerosFolder + zerosFileNames[i];
		if(FLAGS_replace)
			place::analyzePlacement(fpPyramid, erodedFpPyramid, scanName, rotationFile, zerosFile);
		else if(!place::reshowPlacement(scanName, rotationFile, zerosFile)) {
			if(fpPyramid.size() == 0)
				place::createFPPyramids(floorPlan, fpPyramid,  erodedFpPyramid);
			place::analyzePlacement(fpPyramid, erodedFpPyramid, scanName, rotationFile, zerosFile);
		}
	}

	if(FLAGS_previewOut){
		std::vector<moreInfo> scoreInfo;
		confidence::findMostConfidentPlacements(scoreInfo);
		std::cout << scoreInfo.size() << std::endl;
		for(auto & m : scoreInfo) {
			const std::string scanName = FLAGS_dmFolder + pointFileNames[m.scanNum];
			const std::string rotationFile = FLAGS_rotFolder + rotationFileNames[m.scanNum];
			const std::string zerosFile = FLAGS_zerosFolder + zerosFileNames[m.scanNum];
			place::displayMostConfidentScan(scanName, rotationFile, zerosFile, m.s);
		}
	}
	
	return 0;
}

void place::analyzePlacement(const std::vector<Eigen::SparseMatrix<double> > & fpPyramid,
	const std::vector<Eigen::SparseMatrix<double> > & erodedFpPyramid,
	const std::string & scanName, const std::string & rotationFile,
	const std::string & zerosFile) {


	if(!FLAGS_quiteMode) {
		std::cout << rotationFile << std::endl;
		std::cout << scanName << std::endl;
	}
	


	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(6,6));
	std::vector<cv::Mat> rotatedScans;
	place::loadInScans(scanName, rotationFile, zerosFile, rotatedScans);
	std::vector<cv::Mat> erodedScans;

	for(auto & scan : rotatedScans) {
		cv::Mat dst (scan.rows, scan.cols, CV_8UC1);
		cv::erode(scan, dst, element);
		erodedScans.push_back(dst);
	}

	std::vector<Eigen::SparseMatrix<double> > erodedSSparse;
	std::vector<Eigen::SparseMatrix<double> > rSSparse;
	for (int i = 0; i < rotatedScans.size(); ++i) {
		Eigen::SparseMatrix<double> eroded, scan;

		place::scanToSparse(erodedScans[i], eroded);
		place::scanToSparse(rotatedScans[i], scan);

		erodedSSparse.push_back(eroded);
		rSSparse.push_back(scan);
		place::distanceTransform(scan);
	}

	std::vector<std::vector<Eigen::SparseMatrix<double> > > rSSparsePyramid = {rSSparse};
	createPyramid(rSSparsePyramid);
	
	std::vector<std::vector<Eigen::SparseMatrix<double> > > erodedSparsePyramid = {erodedSSparse};
	createPyramid(erodedSparsePyramid);

	std::vector<std::vector<Eigen::SparseMatrix<double> > > erodedSparsePyramidTrimmed;
	std::vector<std::vector<Eigen::SparseMatrix<double> > > rSSparsePyramidTrimmed;
	trimScanPryamids(rSSparsePyramid, rSSparsePyramidTrimmed, 
		erodedSparsePyramid, erodedSparsePyramidTrimmed);


	std::vector<Eigen::Vector3i> pointsToAnalyze;
	for(int k = 0; k < rSSparse.size(); ++k) {
		const int xStop = fpPyramid[FLAGS_numLevels].cols() 
		- rSSparsePyramidTrimmed[FLAGS_numLevels][k].cols();
		const int yStop = fpPyramid[FLAGS_numLevels].rows()
		- rSSparsePyramidTrimmed[FLAGS_numLevels][k].rows();
		pointsToAnalyze.reserve(xStop*yStop*(k+1));
		for (int i = 0; i < xStop; ++i) {
			for (int j = 0; j < yStop; ++j) {
				pointsToAnalyze.push_back(Eigen::Vector3i (i,j,k));
			}
		}
	}
	if(FLAGS_debugMode)
		loadInTruePlacement(scanName);
	
	std::vector<posInfo> scores;
	std::vector<Eigen::MatrixXd> scoreMatricies;
	std::vector<int> localMinima;
	Eigen::Vector4i rows, cols;
	for (int k = FLAGS_numLevels; k >= 0; --k) {
		if(FLAGS_debugMode) {
			levelNum = k;
			for(auto v : truePlacement) {
				v[0]/=pow(2,k);
				v[1]/=pow(2,k);
				pointsToAnalyze.push_back(v);
			}
		}
		
		findPlacementPointBasedV2(fpPyramid[k], rSSparsePyramidTrimmed[k],
			erodedFpPyramid[k], erodedSparsePyramidTrimmed[k], 
			scores, pointsToAnalyze);
		/*for (int i = 0; i < 4; ++i) {
			rows[i] = fpPyramid[k].rows() - rSSparsePyramidTrimmed[k][i].rows();
			cols[i] = fpPyramid[k].cols() - rSSparsePyramidTrimmed[k][i].cols();
		}
		blurMinima(scores, rows, cols, scoreMatricies);*/
		if(k!=0) {
			findLocalMinima(scores, localMinima, 1.0);
			findGlobalMinima(scores, localMinima);
			findPointsToAnalyze(scores, localMinima, pointsToAnalyze);
		}
		
	}

	findLocalMinima(scores, localMinima, 2.5);
	findGlobalMinima(scores, localMinima);

	if(FLAGS_save) {
		const std::string placementName = FLAGS_preDone + scanName.substr(scanName.find("_")-3, 3) 
		+ "_placement_" + scanName.substr(scanName.find(".")-3, 3) + ".txt";
		
		savePlacement(scores, localMinima, placementName);
	}
	
	if(FLAGS_visulization || FLAGS_previewOut)
		place::displayOutput(rSSparsePyramidTrimmed[0], localMinima, scores);
	if(FLAGS_debugMode) {
		displayTruePlacement(rSSparsePyramidTrimmed[0], scores);
	}
}

void place::findLocalMinima(const std::vector<posInfo> & scores, std::vector<int> & localMinima, 
	const float bias) {
	localMinima.clear();
	double averageScore = 0;
	for(auto & info : scores) {
		averageScore += info.score;
	}
	averageScore /= scores.size();
	double sigScores = 0;
	for(auto & info : scores) {
		sigScores += (info.score - averageScore)*(info.score - averageScore);
	}
	sigScores /= (scores.size() - 1);
	sigScores = sqrt(sigScores);
	if(!FLAGS_quiteMode) {
		std::cout << "Average         Sigma" << std::endl;
		std::cout << averageScore << "         " << sigScores << std::endl; 
	}
	

	const double cutOff = averageScore - bias*sigScores;
	int i;
	// #pragma omp parallel for shared(i, scores) reduction(merge: localMinima) reduction(merge: globalMins)
	for (i = 1; i < (scores.size() - 1); ++i) {
		double lHS = scores[i-1].score - scores[i].score;
		double rHS = scores[i].score - scores[i+1].score;
		if( lHS >= 0 && rHS <= 0 && scores[i].score < cutOff)
			localMinima.push_back(i);
	}
}

void place::createPyramid(std::vector<Eigen::SparseMatrix<double> > & pyramid) {

	std::vector<Eigen::Triplet<double> > tripletList;
	
	for (int i = 0; i < FLAGS_numLevels; ++i) {
		Eigen::SparseMatrix<double> & currentLevel = pyramid[i];
		Eigen::SparseMatrix<double> newLevel (floor(currentLevel.rows()/2), 
			floor(currentLevel.cols()/2));

		Eigen::MatrixXd currentLevelNS = Eigen::MatrixXd(currentLevel);
		
		for (int j = 0; j < (currentLevelNS.rows() -1); j+=2) {
			for (int k = 0; k < (currentLevelNS.cols() -1); k+=2) {
				double maxV = std::max(currentLevelNS(j,k),std::max(currentLevelNS(j,k+1),
					std::max(currentLevelNS(j+1,k), currentLevelNS(j+1,k+1))));
				tripletList.push_back(Eigen::Triplet<double> (j/2, k/2, maxV));
			}
		}

		newLevel.setFromTriplets(tripletList.begin(), tripletList.end());
		pyramid.push_back(newLevel);
		tripletList.clear();
	}

	if(FLAGS_visulization) {
		for(auto & level: pyramid) {
			cv::Mat test (level.rows(), level.cols(), CV_8UC1, cv::Scalar::all(255));
			for (int k = 0; k < level.outerSize(); ++k) {
				for (Eigen::SparseMatrix<double>::InnerIterator it (level, k); it; ++it) {
					test.at<uchar>(it.row(), it.col()) = 255 - it.value()*255;
				}
			}
			cvNamedWindow("Preview", CV_WINDOW_NORMAL);
			cv::imshow("Preview", test);
			cv::waitKey(0);
		}
	}
}

void place::createPyramid( std::vector<std::vector<Eigen::SparseMatrix<double> > > & pyramid) {

	std::vector<Eigen::Triplet<double> > tripletList;
	
	for (int i = 0; i < FLAGS_numLevels; ++i) {
		std::vector<Eigen::SparseMatrix<double> > newLevel;
		for(auto & scan : pyramid[i]) {
			Eigen::SparseMatrix<double> newScan (floor(scan.rows()/2), floor(scan.cols()/2));
			Eigen::MatrixXd scanNS = Eigen::MatrixXd(scan);

			for (int j = 0; j < (scan.rows()-1); j+=2) {
				for (int k = 0; k < (scan.cols()-1); k+=2) {
					double maxV = std::max(scanNS(j,k),std::max(scanNS(j,k+1),
					std::max(scanNS(j+1,k), scanNS(j+1,k+1))));
					tripletList.push_back(Eigen::Triplet<double> (j/2, k/2, maxV));
				}
			}

			newScan.setFromTriplets(tripletList.begin(), tripletList.end());
			newLevel.push_back(newScan);
			tripletList.clear();
		}

		pyramid.push_back(newLevel);
	}

	if(FLAGS_visulization) {
		for(auto level : pyramid) {
			for(auto scan : level) {
				cv::Mat test (scan.rows(), scan.cols(), CV_8UC1, cv::Scalar::all(255));
				for (int k = 0; k < scan.outerSize(); ++k) {
					for (Eigen::SparseMatrix<double>::InnerIterator it (scan, k); it; ++it) {
						test.at<uchar>(it.row(), it.col()) = 255 - it.value()*255;
					}
				}
				cvNamedWindow("Preview", CV_WINDOW_NORMAL);
				cv::imshow("Preview", test);
				cv::waitKey(0);
			}
		}
	}

	
}

void place::trimScanPryamids(const std::vector<std::vector<Eigen::SparseMatrix<double> > > & rSSparsePyramid,
	std::vector<std::vector<Eigen::SparseMatrix<double> > > & rSSparsePyramidTrimmed, 
	const std::vector<std::vector<Eigen::SparseMatrix<double> > > & erodedSparsePyramid,
	std::vector<std::vector<Eigen::SparseMatrix<double> > > & erodedSparsePyramidTrimmed) {

	std::vector<Eigen::Triplet<double> > tripletList;
	for(int level = 0; level < rSSparsePyramid.size(); ++level) {
		std::vector<Eigen::SparseMatrix<double> > levelTrimmed;
		std::vector<Eigen::SparseMatrix<double> > erodedLevelTrimed;
		for(int i = 0; i < rSSparsePyramid[level].size(); ++i) {
			const Eigen::SparseMatrix<double> * scan = &rSSparsePyramid[level][i];
			const Eigen::SparseMatrix<double> * erodedScan = &erodedSparsePyramid[level][i];

			Eigen::SparseMatrix<double> scanThreshHolded (scan->rows(), scan->cols());
			Eigen::SparseMatrix<double> eScanThreshHolded (erodedScan->rows(), erodedScan->cols());

			for (int k = 0; k < scan->outerSize(); ++k) {
				for (Eigen::SparseMatrix<double>::InnerIterator it (*scan, k); it; ++it) {
					if(it.value() > 0.75) {
						tripletList.push_back(Eigen::Triplet<double> (it.row(), it.col(), it.value()));
					}
				}
			}
			scanThreshHolded.setFromTriplets(tripletList.begin(), tripletList.end());
			tripletList.clear();

			for (int k = 0; k < erodedScan->outerSize(); ++k) {
				for (Eigen::SparseMatrix<double>::InnerIterator it (*erodedScan, k); it; ++it) {
					if(it.value() > 0.75) {
						tripletList.push_back(Eigen::Triplet<double> (it.row(), it.col(), it.value()));
					}
				}
			}
			eScanThreshHolded.setFromTriplets(tripletList.begin(), tripletList.end());
			tripletList.clear();


			int minRow, minCol, maxRow, maxCol;
			maxRow = maxCol = 0;
			minRow = scanThreshHolded.rows();
			minCol = scanThreshHolded.cols();

			for (int k = 0; k < scanThreshHolded.outerSize(); ++k) {
				for (Eigen::SparseMatrix<double>::InnerIterator it (scanThreshHolded, k); it; ++it) {
					if(it.value() !=0) {
						maxRow = std::max(maxRow, it.row());
						minRow = std::min(minRow, it.row());

						maxCol = std::max(maxCol, it.col());
						minCol = std::min(minCol, it.col());
					}
				}
			}

			Eigen::SparseMatrix<double> trimmed = scanThreshHolded.block(minRow, minCol, 
				maxRow - minRow, maxCol - minCol);
			Eigen::SparseMatrix<double> erodedTrimmed = eScanThreshHolded.block(minRow, minCol, 
				maxRow - minRow, maxCol - minCol);
		
			
			if(level == 0) {
				zeroZero[i][0] -= minCol;
				zeroZero[i][1] -= minRow;
			}
			
			trimmed.makeCompressed();
			levelTrimmed.push_back(trimmed);
			erodedLevelTrimed.push_back(erodedTrimmed);
		}
		
		rSSparsePyramidTrimmed.push_back(levelTrimmed);
		erodedSparsePyramidTrimmed.push_back(erodedLevelTrimed);
	}

	if(FLAGS_visulization) {
		for(auto & level: rSSparsePyramidTrimmed) {
			for(auto & scan : level) {
				cv::Mat test (scan.rows(), scan.cols(), CV_8UC1, cv::Scalar::all(255));
				for (int k = 0; k < scan.outerSize(); ++k) {
					for (Eigen::SparseMatrix<double>::InnerIterator it (scan, k); it; ++it) {
						test.at<uchar>(it.row(), it.col()) = 255 - it.value()*255;
					}
				}
				cvNamedWindow("Preview", CV_WINDOW_NORMAL);
				cv::imshow("Preview", test);
				cv::waitKey(0);
			}
		}
	}
}

void place::findPlacementPointBasedV2(const Eigen::SparseMatrix<double> & fp, 
	const std::vector<Eigen::SparseMatrix<double> > & scans,const Eigen::SparseMatrix<double> & fpE, 
	const std::vector<Eigen::SparseMatrix<double> > & scansE, std::vector<posInfo> & scores,
	const std::vector<Eigen::Vector3i> & points) {
	
	if(!FLAGS_quiteMode)
		std::cout << points.size() << std::endl;

	scores.clear();
	scores.reserve(points.size());


	#pragma omp parallel shared(scores) 
	{
		std::vector<posInfo> privateScores;
		#pragma omp for nowait schedule(static)
		for(int i = 0; i< points.size(); ++i) {
			
			const Eigen::Vector3i point = points[i];
			const int scanIndex = point[2];
			const int xStop = fp.cols() - scans[scanIndex].cols();
			const int yStop = fp.rows() - scans[scanIndex].rows();

			if(point[0] < 0 || point[0] >=xStop)
				continue;
			if(point[1] < 0 || point[1] >= yStop)
				continue;
			
			const Eigen::SparseMatrix<double> & currentScan = scans[scanIndex];

			Eigen::SparseMatrix<double> currentFP = fp.block(point[1], point[0], 
				currentScan.rows(), currentScan.cols());
			currentFP.prune(1.0);

			if(currentFP.nonZeros() < currentScan.nonZeros())
				continue;


			double score;
			double diffEntropy = 0;
			double scanFPsetDiff = 0;
			double fpScanSetDiff = 0;

			/*if(point[0] == static_cast<int>(1187.0/pow(2, levelNum)) &&
				point[1] == static_cast<int>(183.0/pow(2,levelNum))) {
				cv::Mat test (diff.rows(), diff.cols(), CV_8UC1, cv::Scalar::all(128));
				for (int i = 0; i < diff.outerSize(); ++i) {
					for(Eigen::SparseMatrix<double>::InnerIterator it (diff, i); it; ++it) {
						
						test.at<uchar>(it.row(), it.col()) = std::max(0,std::min(255,
							static_cast<int>(it.value()*128 +128)));
					}
				}
				cvNamedWindow("Preview", CV_WINDOW_NORMAL);
				cv::imshow("Preview", test);
				

				cv::Mat test2 (currentFP.rows(), currentFP.cols(), CV_8UC1, cv::Scalar::all(128));
				for (int i = 0; i < currentFP.outerSize(); ++i) {
					for(Eigen::SparseMatrix<double>::InnerIterator it (currentFP, i); it; ++it) {
						
						test2.at<uchar>(it.row(), it.col()) = std::max(0,std::min(255,
							static_cast<int>(it.value()*128 +128)));
					}
				}
				cvNamedWindow("Preview2", CV_WINDOW_NORMAL);
				cv::imshow("Preview2", test2);

				cv::Mat test3 (currentScan.rows(), currentScan.cols(), CV_8UC1, cv::Scalar::all(128));
				for (int i = 0; i < currentScan.outerSize(); ++i) {
					for(Eigen::SparseMatrix<double>::InnerIterator it (currentScan, i); it; ++it) {
						
						test3.at<uchar>(it.row(), it.col()) = std::max(0,std::min(255,
							static_cast<int>(it.value()*128 +128)));
					}
				}
				cvNamedWindow("Preview3", CV_WINDOW_NORMAL);
				cv::imshow("Preview3", test3);
				cv::waitKey(0);
			}*/
			
			
			if(FLAGS_metricNumber == 1) {
				const double fpScore = currentFP.squaredNorm();
				Eigen::SparseMatrix<double> diff = currentScan - currentFP;
				const double diffScore = diff.squaredNorm();

				for(int i = 0; i < diff.outerSize(); ++i) {
					for(Eigen::SparseMatrix<double>::InnerIterator it (diff, i); it; ++it) {
						if(it.value() != 0) {
							const double tmp = it.value()*it.value()/diffScore;
							diffEntropy -= tmp*log(tmp);
						}
					}
				}

				const double placement = diffScore - fpScore;
				score = (placement > 0) ? placement*diffEntropy
				: placement/diffEntropy;
			} else if(FLAGS_metricNumber == 2 || FLAGS_metricNumber == 3) {
				Eigen::SparseMatrix<double> currentFPE = fpE.block(point[1], point[0], 
				currentScan.rows(), currentScan.cols());
				currentFPE.prune(1.0);

				Eigen::SparseMatrix<double> diff = currentScan - currentFPE;

				for(int i = 0; i < diff.outerSize(); ++i) {
					for(Eigen::SparseMatrix<double>::InnerIterator it (diff, i); it; ++it) {
						if(it.value() > 0) {
							scanFPsetDiff += it.value();
						}
					}
				}

				diff = currentFP - scansE[scanIndex];
				for(int i = 0; i < diff.outerSize(); ++i) {
					for(Eigen::SparseMatrix<double>::InnerIterator it (diff, i); it; ++it) {
						if(it.value() > 0) {
							fpScanSetDiff += it.value();
						}
					}
				}
				if(FLAGS_metricNumber == 2) {
					score = scanFPsetDiff + 
						std::max(0.0, fpScanSetDiff - 0.5*scanFPsetDiff)*0.05;
				} else {
					score = 3.0/(2.0*currentScan.nonZeros()/scanFPsetDiff 
						+ currentFP.nonZeros()/fpScanSetDiff);
				}

			} else {
				
			}
			posInfo tmp;
			tmp.x = point[0];
			tmp.y = point[1];
			tmp.score = score;
			tmp.scanFP = scanFPsetDiff;
			tmp.fpScan = fpScanSetDiff;
			tmp.scanPixels = currentScan.nonZeros();
			tmp.fpPixels = currentFP.nonZeros();
			tmp.rotation = scanIndex;
			tmp.entropy = diffEntropy;
			privateScores.push_back(tmp);
			

		}

		#pragma omp for schedule(static) ordered
		for (int i = 0; i < omp_get_num_threads(); ++i) {
			#pragma omp ordered
			scores.insert(scores.end(), privateScores.begin(), privateScores.end());
		}
	}

	if(!FLAGS_quiteMode)
		std::cout << "Done: " << scores.size() << std::endl;
}

void place::findPointsToAnalyze(const std::vector<posInfo> & scores, const std::vector<int> & localMinima,
	std::vector<Eigen::Vector3i> & pointsToAnalyze) {
	pointsToAnalyze.clear();
	pointsToAnalyze.reserve(localMinima.size()*4 + globalMins.size()*4);

	for(auto & index : localMinima) {
		posInfo minInfo = scores[index];
		int x = minInfo.x;
		int y = minInfo.y;
		int rotIndex = minInfo.rotation;
		pointsToAnalyze.push_back(Eigen::Vector3i(2*x,2*y,rotIndex));
		pointsToAnalyze.push_back(Eigen::Vector3i(2*x + 1,2*y,rotIndex));
		pointsToAnalyze.push_back(Eigen::Vector3i(2*x,2*y + 1,rotIndex));
		pointsToAnalyze.push_back(Eigen::Vector3i(2*x + 1,2*y + 1,rotIndex));
	}

	for(auto & globMin : globalMins) {
		posInfo minInfo = scores[globMin];
		int x = minInfo.x;
		int y = minInfo.y;
		int rotIndex = minInfo.rotation;
		pointsToAnalyze.push_back(Eigen::Vector3i(2*x,2*y,rotIndex));
		pointsToAnalyze.push_back(Eigen::Vector3i(2*x + 1,2*y,rotIndex));
		pointsToAnalyze.push_back(Eigen::Vector3i(2*x,2*y + 1,rotIndex));
		pointsToAnalyze.push_back(Eigen::Vector3i(2*x + 1,2*y + 1,rotIndex));
	}
	
}

void place::findGlobalMinima(const std::vector<posInfo> & scores, const std::vector<int> & localMinima) {
	globalMins.clear();

	double minScore = 5e30;
	for(auto & info : scores) {
		minScore = std::min(info.score, minScore);
	}

	if(!FLAGS_quiteMode)
		std::cout << "Min score: " << minScore << std::endl;

	for(int i = 0; i < scores.size(); ++i) {
		if(scores[i].score >=0) {
			if(scores[i].score == minScore)
				if(notInLocalMin(i, localMinima))
					globalMins.push_back(i);
		}
	}
}

void place::createFPPyramids(const cv::Mat & floorPlan, 
	std::vector<Eigen::SparseMatrix<double> > & fpPyramid,  
	std::vector<Eigen::SparseMatrix<double> > &erodedFpPyramid) {

	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(6,6));
	cv::Mat fpEroded (floorPlan.rows, floorPlan.cols, CV_8UC1);
	cv::erode(floorPlan, fpEroded, element);

	Eigen::SparseMatrix<double> fpSparse, erodedFpSparse;
	place::scanToSparse(floorPlan, fpSparse);
	place::scanToSparse(fpEroded, erodedFpSparse);

	fpPyramid = {fpSparse};
	createPyramid(fpPyramid);

	erodedFpPyramid = {erodedFpSparse};
	createPyramid(erodedFpPyramid);
}

bool place::notInLocalMin(const int i, const std::vector<int> & localMinima) {
	if(localMinima.size() == 0)
		return true;
	if(localMinima.size() == 1)
		return !(i == localMinima[0]);

	if(i == localMinima[localMinima.size()/2]) {
		return false;
	}else if( i < localMinima[localMinima.size()/2]) {
		return notInLocalMin(i, 
			std::vector<int>(localMinima.begin(), localMinima.begin() + localMinima.size()/2));
	} else {
		return notInLocalMin(i, 
			std::vector<int>(localMinima.begin() + localMinima.size()/2, localMinima.end()));
	}
}

Eigen::MatrixXd place::distanceTransform(const Eigen::SparseMatrix<double> & image) {
	cv::Mat dist;
	cv::distanceTransform(sparseToImage(image), dist, CV_DIST_L2, 0);
	cv::normalize(dist, dist, 0, 1.0, cv::NORM_MINMAX);

	Eigen::MatrixXd out;
	cv::cv2eigen(dist, out);
	return out;
}


void place::blurMinima(const std::vector<posInfo> & scores, 
	const Eigen::Vector4i & rows, const Eigen::Vector4i & cols,
	std::vector<Eigen::MatrixXd> & scoreMatricies) {
	scoreMatricies.clear();

	for (int i = 0; i < rows.size(); ++i) {
		scoreMatricies.push_back(Eigen::MatrixXd::Zero(rows[i], cols[i]));
	}
	
	Eigen::MatrixXd kernel (5,5);
	const double sigma = 1.25;
	for (int i = -2; i < kernel.rows()-2; ++i) {
		for (int j = -2; j < kernel.cols()-2; ++j) {
			kernel(i+2,j+2) = 1.0/(2*PI*sigma*sigma)*exp(-1.0*static_cast<double>(i*i + j*j)/(2*sigma*sigma));
		}
	}

	const double * kernlPtr = kernel.data();

	for(auto minScore : scores) {
		if(minScore.rotation == 0)
			scoreMatricies[0](minScore.y, minScore.x) = minScore.score;

		if(minScore.rotation == 1)
			scoreMatricies[1](minScore.y, minScore.x) = minScore.score;

		if(minScore.rotation == 2)
			scoreMatricies[2](minScore.y, minScore.x) = minScore.score;

		if(minScore.rotation == 3)
			scoreMatricies[3](minScore.y, minScore.x) = minScore.score;
	}

	for (int k = 0; k < rows.size(); ++k) {
		Eigen::MatrixXd blurredScore = Eigen::MatrixXd::Zero(rows[k], cols[k]);
		for (int i = 2; i < rows[k] - 2; ++i) {
			for (int j = 2; j < cols[k] - 2; ++j) {
				const Eigen::MatrixXd toBlur = scoreMatricies[k].block(i-2, j-2, 5, 5);
				const double * blurPtr = toBlur.data();
				double value = 0;
				for (int a = 0; a < kernel.size(); ++a) {
					value += (*(kernlPtr + a))*(*(blurPtr + a));
				}
				blurredScore(i,j) = value;
			}
		}
		scoreMatricies[k] = blurredScore;
	}
	

	double aveScore = 0;
	int count = 0;
	for (int k = 0; k < rows.size(); ++k) {
		const double * scoreMatrixPtr = scoreMatricies[k].data();
		for (int i = 0; i < scoreMatricies[k].size(); ++i) {
			if(*(scoreMatrixPtr + i) != 0) {
				aveScore += *(scoreMatrixPtr + i);
				count ++;
			}
		}
	}
	
	aveScore /= count;
	double sigScores = 0;
	
	for (int k = 0; k < rows.size(); ++k) {
		const double * scoreMatrixPtr = scoreMatricies[k].data();
		for (int i = 0; i <scoreMatricies[k].size(); ++i) {
			if(*(scoreMatrixPtr + i) != 0) {
				const double tmp = *(scoreMatrixPtr + i) - aveScore;
				sigScores += tmp*tmp;
			}
			
		}
	}

	
	sigScores /= count;
	sigScores = sqrt(sigScores);


	if(FLAGS_visulization) {
		for (int k = 0; k < rows.size(); ++k) {
			cv::Mat blurScore (rows[k], cols[k], CV_8UC1, cv::Scalar::all(255));
			for (int i = 0; i < blurScore.rows; ++i) {
				uchar * dst = blurScore.ptr<uchar>(i);
				for (int j = 0; j < blurScore.cols; ++j) {
					if(scoreMatricies[k](i,j) != 0) {
						const int gray = std::max(0, std::min(255,
							 static_cast<int>(255.0 * (
								((scoreMatricies[k](i,j) - aveScore)/(3.0*sigScores) + 1.0)/2.0))));
						dst[j] = 255 - gray;
					}
				}
			}
			cvNamedWindow("Preview", CV_WINDOW_NORMAL);
			cv::imshow("Preview", blurScore);
			cv::waitKey(0);
		}
		
	} 
}