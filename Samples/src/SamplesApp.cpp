#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"

#include "samples/Samples.h"

using namespace ci;
using namespace ci::app;
using namespace pockets;
using namespace std;

class SamplesApp : public AppNative {
public:
  void prepareSettings( Settings *settings ) override;
	void setup() override;
	void update() override;

  void loadSample( int index );
private:
  pk::SceneRef            mCurrentScene;
  ch::Timeline            mTimeline;
  ci::Timer               mTimer;
  int                     mSceneIndex = 0;
  string                  mSceneName;
  params::InterfaceGlRef  mParams;
};

void SamplesApp::prepareSettings( Settings *settings )
{
  settings->setWindowSize( 1280, 720 );
  settings->enableMultiTouch();
  settings->disableFrameRate();
}

void SamplesApp::setup()
{
#ifndef CINDER_COCOA_TOUCH
  mParams = params::InterfaceGl::create( "Choreograph Samples", ivec2( 240, 100 ) );
  mParams->setPosition( ivec2( getWindowWidth() - 250, 10 ) );
  mParams->addParam( "Sample", SampleNames, &mSceneIndex );
  mParams->addButton( "Next", [this] { loadSample( mSceneIndex + 1 ); } );
  mParams->addButton( "Prev", [this] { loadSample( mSceneIndex - 1 ); } );
  mParams->addButton( "Restart Current", [this] { loadSample( mSceneIndex ); } );
#endif
  // Draw our app first, so samples show up over top.
  getWindow()->getSignalDraw().connect( 0, [this] {
    gl::clear( Color::black() );
#ifndef CINDER_COCOA_TOUCH
    mParams->draw();
#endif
  } );

  loadSample( 0 );
  mTimer.start();
}

void SamplesApp::loadSample( int index )
{
  if( index < 0 ) { index = SampleList.size() - 1; }
  index %= SampleList.size();

  mSceneIndex = index;
  mSceneName = SampleNames[mSceneIndex];

  console() << "Loading Sample: " << mSceneName << endl;
  mCurrentScene = SampleList[mSceneIndex].second();

  mCurrentScene->setup();
  mCurrentScene->connect( getWindow() );
  mCurrentScene->show( getWindow() );

  // Load Next Sample in 10 seconds.
  mTimeline.cue( [this] {
    loadSample( mSceneIndex + 1 );
  }, 6.0f );
}

void SamplesApp::update()
{
  if( mSceneName != SampleNames[mSceneIndex] ) {
    loadSample( mSceneIndex );
  }

  ch::Time dt = mTimer.getSeconds();
  mTimer.start();
  mTimeline.step( dt );
}

CINDER_APP_NATIVE( SamplesApp, RendererGl )
