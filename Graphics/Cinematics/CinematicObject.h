#pragma once

#include <vector>

#include "CinematicPlayer.h"

class CCinematicObjectKeyFrame;
class CElement;
class CXMLTreeNode;

class CCinematicObject : public CCinematicPlayer
{
private:
	std::vector<CCinematicObjectKeyFrame *> m_CinematicObjectKeyFrames;
	std::string m_name;
	size_t m_CurrentKeyFrame;
public:
	CCinematicObject( CXMLTreeNode &TreeNode );
	bool IsOk();
	virtual ~CCinematicObject();
	void AddCinematicObjectKeyFrame( CCinematicObjectKeyFrame *CinematicObjectKeyFrame );
	void Update( float ElapsedTime );
	void Stop();
	std::string getName() { return m_name;  }
	void OnRestartCycle();
};
