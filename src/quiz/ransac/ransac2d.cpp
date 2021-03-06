/* \author Aaron Brown */
// Quiz on implementing simple RANSAC line fitting

#include "../../render/render.h"
#include <unordered_set>
#include "../../processPointClouds.h"
// using templates for processPointClouds so also include .cpp to help linker
#include "../../processPointClouds.cpp"

pcl::PointCloud<pcl::PointXYZ>::Ptr CreateData()
{
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>());
  	// Add inliers
  	float scatter = 0.6;
  	for(int i = -5; i < 5; i++)
  	{
  		double rx = 2*(((double) rand() / (RAND_MAX))-0.5);
  		double ry = 2*(((double) rand() / (RAND_MAX))-0.5);
  		pcl::PointXYZ point;
  		point.x = i+scatter*rx;
  		point.y = i+scatter*ry;
  		point.z = 0;

  		cloud->points.push_back(point);
  	}
  	// Add outliers
  	int numOutliers = 10;
  	while(numOutliers--)
  	{
  		double rx = 2*(((double) rand() / (RAND_MAX))-0.5);
  		double ry = 2*(((double) rand() / (RAND_MAX))-0.5);
  		pcl::PointXYZ point;
  		point.x = 5*rx;
  		point.y = 5*ry;
  		point.z = 0;

  		cloud->points.push_back(point);

  	}
  	cloud->width = cloud->points.size();
  	cloud->height = 1;

  	return cloud;

}

pcl::PointCloud<pcl::PointXYZ>::Ptr CreateData3D()
{
	ProcessPointClouds<pcl::PointXYZ> pointProcessor;
	return pointProcessor.loadPcd("../../../sensors/data/pcd/simpleHighway.pcd");
}


pcl::visualization::PCLVisualizer::Ptr initScene()
{
	pcl::visualization::PCLVisualizer::Ptr viewer(new pcl::visualization::PCLVisualizer ("2D Viewer"));
	viewer->setBackgroundColor (0, 0, 0);
  	viewer->initCameraParameters();
  	viewer->setCameraPosition(0, 0, 15, 0, 1, 0);
  	viewer->addCoordinateSystem (1.0);
  	return viewer;
}

std::unordered_set<int> RansacPlane(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud, int maxIterations, float distanceTol)
{
	std::unordered_set<int> inliersResult;
	srand(time(NULL));

	int best_num_inliers = 0;
	std::unordered_set<int> inliers;

	for (int i = 0; i < maxIterations; i++)
	{
		int num_inliers = 0;

		// Randomly sample subset and fit line
		int idx1 = rand() % cloud->points.size();
		int idx2 = idx1;
		int idx3 = idx1;
		while (idx2 == idx1)
		{
			idx2 = rand() % cloud->points.size();
		}
		while (idx3 == idx1 || idx3 == idx2)
		{
			idx3 = rand() % cloud->points.size();
		}

		// Define required vectors
		Eigen::Vector3f v1(cloud->points[idx2].x - cloud->points[idx1].x,
						   cloud->points[idx2].y - cloud->points[idx1].y,
						   cloud->points[idx2].z - cloud->points[idx1].z);
		Eigen::Vector3f v2(cloud->points[idx3].x - cloud->points[idx1].x,
						   cloud->points[idx3].y - cloud->points[idx1].y,
						   cloud->points[idx3].z - cloud->points[idx1].z);
		Eigen::Vector3f v1xv2 = v1.cross(v2);

		// 4 values define a plane in 3D: Ax + By + Cz + D = 0
		float A, B, C, D;
		A = v1xv2(0);
		B = v1xv2(1);
		C = v1xv2(2);
		D = -(v1xv2(0) * v1(0) + v1xv2(1) * v1(1) + v1xv2(2) * v1(2));

		// Distance
		for (int j = 0; j < cloud->points.size(); j++)
		{
			auto& p = cloud->points[j];
			float d = fabs(A * p.x + B * p.y + C * p.z + D) / sqrt(A * A + B * B + C * C);
			// std::cout << "A, B, C" << A << " " << B << " " << C << " distance: " << d << std::endl;
			if (d < distanceTol)
			{
				inliers.insert(j);
				num_inliers++;
			}
		}
		std::cout << "iter: " << i << ", inliers: " << num_inliers << std::endl;

		if (num_inliers > best_num_inliers)
		{
			inliersResult.clear();
			inliersResult = inliers;
			best_num_inliers = num_inliers;
		}
		inliers.clear();
	}
	
	return inliersResult;

}

std::unordered_set<int> RansacLine(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud, int maxIterations, float distanceTol)
{
	std::unordered_set<int> inliersResult;
	srand(time(NULL));

	int best_num_inliers = 0;
	std::unordered_set<int> inliers;

	for (int i = 0; i < maxIterations; i++)
	{
		int num_inliers = 0;

		// Randomly sample subset and fit line
		int idx1 = rand() % cloud->points.size();
		int idx2 = idx1;
		while (idx2 == idx1)
		{
			idx2 = rand() % cloud->points.size();
		}

		float A, B, C;
		A = cloud->points[idx1].y - cloud->points[idx2].y;
		B = cloud->points[idx2].x - cloud->points[idx1].x;
		C = cloud->points[idx1].x * cloud->points[idx2].y - cloud->points[idx2].x * cloud->points[idx1].y;

		// Distance
		for (int j = 0; j < cloud->points.size(); j++)
		{
			auto& p = cloud->points[j];
			float d = fabs(A * p.x + B * p.y + C) / sqrt(A * A + B * B);
			// std::cout << "A, B, C" << A << " " << B << " " << C << " distance: " << d << std::endl;
			if (d < distanceTol)
			{
				inliers.insert(j);
				num_inliers++;
			}
		}
		std::cout << "iter: " << i << ", inliers: " << num_inliers << std::endl;

		if (num_inliers > best_num_inliers)
		{
			inliersResult.clear();
			inliersResult = inliers;
			best_num_inliers = num_inliers;
		}
		inliers.clear();
	}
	
	return inliersResult;

}

int main ()
{

	// Create viewer
	pcl::visualization::PCLVisualizer::Ptr viewer = initScene();

	// Create data
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud = CreateData3D();
	

	// TODO: Change the max iteration and distance tolerance arguments for Ransac function
	std::unordered_set<int> inliers = RansacPlane(cloud, 50, 0.5);

	pcl::PointCloud<pcl::PointXYZ>::Ptr  cloudInliers(new pcl::PointCloud<pcl::PointXYZ>());
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloudOutliers(new pcl::PointCloud<pcl::PointXYZ>());

	for(int index = 0; index < cloud->points.size(); index++)
	{
		pcl::PointXYZ point = cloud->points[index];
		if(inliers.count(index))
			cloudInliers->points.push_back(point);
		else
			cloudOutliers->points.push_back(point);
	}


	// Render 2D point cloud with inliers and outliers
	if(inliers.size())
	{
		renderPointCloud(viewer,cloudInliers,"inliers",Color(0,1,0));
  		renderPointCloud(viewer,cloudOutliers,"outliers",Color(1,0,0));
	}
  	else
  	{
  		renderPointCloud(viewer,cloud,"data");
  	}
	
  	while (!viewer->wasStopped ())
  	{
  	  viewer->spinOnce ();
  	}
  	
}
