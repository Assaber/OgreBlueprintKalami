#pragma once

#include "OgreCamera.h"
#include "OgreSceneNode.h"
#include "OgreFrameListener.h"

#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>

class CameraMan : public Ogre::FrameListener
{
public:
	enum CameraStyle   /// enumerator values for different styles of camera movement
	{
		CS_FREELOOK,
		CS_ORBIT,
		CS_MANUAL
	};

public:
    CameraMan(Ogre::SceneNode* cam);

    /**
    Swaps the camera on our camera man for another camera.
    */
    void setCamera(Ogre::SceneNode* cam);

    Ogre::SceneNode* getCamera()
    {
        return mCamera;
    }

    /**
    Sets the target we will revolve around. Only applies for orbit style.
    */
    virtual void setTarget(Ogre::SceneNode* target);

    Ogre::SceneNode* getTarget()
    {
        return mTarget;
    }

    /**
    Sets the spatial offset from the target. Only applies for orbit style.
    */
    void setYawPitchDist(Ogre::Radian yaw, Ogre::Radian pitch, Ogre::Real dist);

    /**
    Sets the camera's top speed. Only applies for free-look style.
    */
    void setTopSpeed(Ogre::Real topSpeed)
    {
        mTopSpeed = topSpeed;
    }

    Ogre::Real getTopSpeed()
    {
        return mTopSpeed;
    }

    /**
    Sets the movement style of our camera man.
    */
    virtual void setStyle(CameraStyle style);

    CameraStyle getStyle()
    {
        return mStyle;
    }

    /**
    Manually stops the camera when in free-look mode.
    */
    void manualStop();

    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt) override;

    /**
    Processes key presses for free-look style movement.
    */
    bool keyPressed(QKeyEvent* evt);

    /**
    Processes key releases for free-look style movement.
    */
    bool keyReleased(QKeyEvent* evt);

    /**
    Processes mouse movement differently for each style.
    */
    bool mouseMoved(QMouseEvent* evt);

    bool mouseWheelRolled(QWheelEvent* evt);

    /**
    Processes mouse presses. Only applies for orbit style.
    Left button is for orbiting, and right button is for zooming.
    */
    bool mousePressed(QMouseEvent* evt);

    /**
    Processes mouse releases. Only applies for orbit style.
    Left button is for orbiting, and right button is for zooming.
    */
    bool mouseReleased(QMouseEvent* evt);

    void reset();

protected:
    Ogre::Real getDistToTarget();

    Ogre::SceneNode* mCamera;
    CameraStyle mStyle;
    Ogre::SceneNode* mTarget;
    bool mOrbiting;
    bool mMoving;
    Ogre::Real mTopSpeed;
    Ogre::Vector3 mVelocity;
    bool mGoingForward;
    bool mGoingBack;
    bool mGoingLeft;
    bool mGoingRight;
    bool mGoingUp;
    bool mGoingDown;
    Ogre::Vector3 mOffset;
    float mLastAvailablePos[2] = { 0.0f, 0.0f };
    float mDeltaPos[2] = { 0.0f, 0.0f };
};