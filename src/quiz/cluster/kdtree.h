/* \author Aaron Brown */
// Quiz on implementing kd tree

#include "../../render/render.h"


// Structure to represent node of kd tree
struct Node
{
	std::vector<float> point;
	int id;
	Node* left;
	Node* right;

	Node(std::vector<float> arr, int setId)
	:	point(arr), id(setId), left(NULL), right(NULL)
	{}
};

struct KdTree
{
	Node* root;

	KdTree()
	: root(NULL)
	{}

	void insertNode(Node** node, uint depth, std::vector<float> point, int id)
	{
		if (*node == NULL)
		{
			*node = new Node(point, id);
		}
		else
		{
			uint dim = depth % 2;
			if (point[dim] > (*node)->point[dim])
			{
				insertNode(&(*node)->right, depth + 1, point, id);
			}
			else
			{
				insertNode(&(*node)->left, depth + 1, point, id);
			}
		}
	}

	void insert(std::vector<float> point, int id)
	{
		insertNode(&root, 0, point, id);
	}

	bool nodeInBox(Node* node, std::vector<float> target, float distanceTol)
	{
		return (fabs(target[0] - node->point[0]) < distanceTol &&
				fabs(target[1] - node->point[1]) < distanceTol);
	}

	// Euclidean distance
	float distanceToNode(Node* node, std::vector<float> target)
	{
		return sqrt((node->point[0] - target[0]) * (node->point[0] - target[0]) + 
					(node->point[1] - target[1]) * (node->point[1] - target[1]));
	}

	void searchNode(Node* node, uint depth, std::vector<float> target, float distanceTol, std::vector<int>& ids)
	{
		if (node != NULL)
		{
			if (nodeInBox(node, target, distanceTol))
			{
				if (distanceToNode(node, target) < distanceTol)
				{
					ids.push_back(node->id);
				}
			}
			uint dim = depth % 2;
			if (target[dim] - distanceTol < node->point[dim])
			{
				searchNode(node->left, depth + 1, target, distanceTol, ids);
			}
			if (target[dim] + distanceTol > node->point[dim])
			{
				searchNode(node->right, depth + 1, target, distanceTol, ids);
			}
		}
	}

	// return a list of point ids in the tree that are within distance of target
	std::vector<int> search(std::vector<float> target, float distanceTol)
	{
		std::vector<int> ids;
		searchNode(root, 0, target, distanceTol, ids);
		return ids;
	}
	

};




