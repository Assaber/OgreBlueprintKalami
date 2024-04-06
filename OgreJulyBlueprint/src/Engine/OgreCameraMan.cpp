#include "OgreCameraMan.h"
#include "OgreSceneManager.h"

CameraMan::CameraMan(Ogre::SceneNode *cam)
    : mCamera(0)
    , mStyle(CS_MANUAL)
    , mTarget(0)
    , mOrbiting(false)
    , mMoving(false)
    , mTopSpeed(60)
    , mVelocity(Ogre::Vector3::ZERO)
    , mGoingForward(false)
    , mGoingBack(false)
    , mGoingLeft(false)
    , mGoingRight(false)
    , mGoingUp(false)
    , mGoingDown(false)
    , mOffset(0, 0, 0)
{

    setCamera(cam);
    setStyle(CS_ORBIT);
}

void CameraMan::setCamera(Ogre::SceneNode *cam)
{
    mCamera = cam;
}

void CameraMan::setTarget(Ogre::SceneNode *target)
{
    if (target == mTarget)
        return;

    mTarget = target;
}

void CameraMan::setYawPitchDist(Ogre::Radian yaw, Ogre::Radian pitch, Ogre::Real dist)
{
    OgreAssert(mTarget, "no target set");

    mOffset = Ogre::Vector3::ZERO;
    mCamera->setPosition(mTarget->_getDerivedPosition());
    mCamera->setOrientation(mTarget->_getDerivedOrientation());
    mCamera->yaw(yaw);
    mCamera->pitch(-pitch);
    mCamera->translate(Ogre::Vector3(0, 0, dist), Ogre::Node::TS_LOCAL);
}

void CameraMan::setStyle(CameraStyle style)
{
    if (mStyle != CS_ORBIT && style == CS_ORBIT)
    {
        setTarget(mTarget ? mTarget : mCamera->getCreator()->getRootSceneNode());
        mCamera->setFixedYawAxis(true); // also fix axis with lookAt calls
        manualStop();

        // try to replicate the camera configuration
        Ogre::Real dist = getDistToTarget();
        const Ogre::Quaternion& q = mCamera->getOrientation();
        setYawPitchDist(q.getYaw(), q.getPitch(), dist == 0 ? 150 : dist); // enforce some distance
    }
    else if (mStyle != CS_FREELOOK && style == CS_FREELOOK)
    {
        mCamera->setFixedYawAxis(true); // also fix axis with lookAt calls
    }
    else if (mStyle != CS_MANUAL && style == CS_MANUAL)
    {
        manualStop();
    }
    mStyle = style;
    mCamera->setAutoTracking(false);
}

void CameraMan::manualStop()
{
    if (mStyle == CS_FREELOOK)
    {
        mGoingForward = false;
        mGoingBack = false;
        mGoingLeft = false;
        mGoingRight = false;
        mGoingUp = false;
        mGoingDown = false;
        mVelocity = Ogre::Vector3::ZERO;
    }
}

bool CameraMan::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
    if (mStyle == CS_ORBIT)
    {
        // build our acceleration vector based on keyboard input composite
        Ogre::Vector3 accel = Ogre::Vector3::ZERO;
        Ogre::Matrix3 axes = mCamera->getLocalAxes();
        if (mGoingForward) accel -= axes.GetColumn(2);
        if (mGoingBack) accel += axes.GetColumn(2);
        if (mGoingRight) accel += axes.GetColumn(0);
        if (mGoingLeft) accel -= axes.GetColumn(0);
        if (mGoingUp) accel += axes.GetColumn(1);
        if (mGoingDown) accel -= axes.GetColumn(1);

        // if accelerating, try to reach top speed in a certain time
        Ogre::Real topSpeed = mTopSpeed;
        if (accel.squaredLength() != 0)
        {
            accel.normalise();
            mVelocity += accel * topSpeed * evt.timeSinceLastFrame * 10;
        }
        // if not accelerating, try to stop in a certain time
        else mVelocity -= mVelocity * evt.timeSinceLastFrame * 10;

        Ogre::Real tooSmall = std::numeric_limits<Ogre::Real>::epsilon();

        // keep camera velocity below top speed and above epsilon
        if (mVelocity.squaredLength() > topSpeed * topSpeed)
        {
            mVelocity.normalise();
            mVelocity *= topSpeed;
        }
        else if (mVelocity.squaredLength() < tooSmall * tooSmall)
            mVelocity = Ogre::Vector3::ZERO;

        if (mVelocity != Ogre::Vector3::ZERO) mCamera->translate(mVelocity * evt.timeSinceLastFrame);
    }

    return true;
}

bool CameraMan::keyPressed(QKeyEvent* evt)
{
	if (mStyle == CS_ORBIT)
	{
		int key = evt->key();
		if (key == Qt::Key_W || key == Qt::Key_Up) mGoingForward = true;
		else if (key == Qt::Key_S || key == Qt::Key_Down) mGoingBack = true;
		else if (key == Qt::Key_A || key == Qt::Key_Left) mGoingLeft = true;
		else if (key == Qt::Key_D || key == Qt::Key_Right) mGoingRight = true;
		else if (key == Qt::Key_PageUp) mGoingUp = true;
		else if (key == Qt::Key_PageDown) mGoingDown = true;
	}

    evt->accept();
	return true;
}

bool CameraMan::keyReleased(QKeyEvent* evt)
{
	if (mStyle == CS_ORBIT)
	{
        int key = evt->key();
		if (key == Qt::Key_W || key == Qt::Key_Up) mGoingForward = false;
		else if (key == Qt::Key_S || key == Qt::Key_Down) mGoingBack = false;
		else if (key == Qt::Key_A || key == Qt::Key_Left) mGoingLeft = false;
		else if (key == Qt::Key_D || key == Qt::Key_Right) mGoingRight = false;
		else if (key == Qt::Key_PageUp) mGoingUp = false;
		else if (key == Qt::Key_PageDown) mGoingDown = false;
	}

    evt->accept();
	return true;
}

Ogre::Real CameraMan::getDistToTarget()
{
    Ogre::Vector3 offset = mCamera->getPosition() - mTarget->_getDerivedPosition() - mOffset;
    return offset.length();
}

bool CameraMan::mouseMoved(QMouseEvent* evt)
{
    mDeltaPos[0] = evt->localPos().x() - mLastAvailablePos[0];
    mDeltaPos[1] = evt->localPos().y() - mLastAvailablePos[1];

	if (mStyle == CS_ORBIT)
	{
		Ogre::Real dist = getDistToTarget();

		if (mOrbiting)   // yaw around the target, and pitch locally
		{
			mCamera->setPosition(mTarget->_getDerivedPosition() + mOffset);

			//mCamera->yaw(Ogre::Degree(-mDeltaPos[0] * 0.25f), Ogre::Node::TS_PARENT);
			
            Ogre::Quaternion q = mCamera->getOrientation();
            q = Ogre::Quaternion(Ogre::Radian(-mDeltaPos[0] * 0.01f), Ogre::Vector3::UNIT_Y) * q;
            q.normalise();
            mCamera->setOrientation(q);
            mCamera->pitch(Ogre::Degree(-mDeltaPos[1] * 0.25f));
			// don't let the camera go over the top or around the bottom of the target
            mCamera->translate(Ogre::Vector3(0, 0, dist), Ogre::Node::TS_LOCAL);
		}
		else if (mMoving)  // move the camera along the image plane
		{
			Ogre::Vector3 delta = mCamera->getOrientation() * Ogre::Vector3(-mDeltaPos[0], mDeltaPos[1], 0);
			// the further the camera is, the faster it moves
			delta *= dist / 1000.0f;
			mOffset += delta;
			mCamera->translate(delta);
		}
	}
	else if (mStyle == CS_FREELOOK)
	{
		mCamera->yaw(Ogre::Degree(-mDeltaPos[0] * 0.15f), Ogre::Node::TS_PARENT);
		mCamera->pitch(Ogre::Degree(-mDeltaPos[1] * 0.15f));
	}

    mLastAvailablePos[0] = evt->localPos().x();
    mLastAvailablePos[1] = evt->localPos().y();

    evt->accept();
	return true;
}

bool CameraMan::mouseWheelRolled(QWheelEvent* evt)
{
	if (mStyle == CS_ORBIT && evt->delta() != 0)
	{
		Ogre::Real dist = (mCamera->getPosition() - mTarget->_getDerivedPosition()).length();
		mCamera->translate(Ogre::Vector3(0, 0, -evt->delta() * 0.002f * dist), Ogre::Node::TS_LOCAL);
	}

    evt->accept();
    return true;
}

bool CameraMan::mousePressed(QMouseEvent* evt)
{
	if (mStyle == CS_ORBIT)
	{
		if (evt->button() == Qt::RightButton) 
            mOrbiting = true;
		else if (evt->button() == Qt::MidButton) 
            mMoving = true;
	}

    mLastAvailablePos[0] = evt->localPos().x();
    mLastAvailablePos[1] = evt->localPos().y();

    evt->accept();
	return true;
}

bool CameraMan::mouseReleased(QMouseEvent* evt)
{
	if (mStyle == CS_ORBIT)
	{
		if (evt->button() == Qt::RightButton) 
            mOrbiting = false;
		else if (evt->button() == Qt::MidButton) 
            mMoving = false;
	}

    evt->accept();
    return true;
}

void CameraMan::reset()
{
    mOffset = Ogre::Vector3::ZERO;
}
