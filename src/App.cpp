#include "App.h"

#include <config/VRDataIndex.h>

using namespace basicgraphics;
using namespace std;
using namespace glm;

App::App(int argc, char** argv) : VRApp(argc, argv)
{
}

App::~App()
{
	shutdown();
}

void App::onButtonDown(const VRButtonEvent &event) {
    turntable->onButtonDown(event);
}

void App::onButtonUp(const VRButtonEvent &event) {
    turntable->onButtonUp(event);
}

void App::onCursorMove(const VRCursorEvent &event) {
    turntable->onCursorMove(event);
}

void App::onRenderGraphicsContext(const VRGraphicsState &renderState) {
    // This routine is called once per graphics context at the start of the
    // rendering process.  So, this is the place to initialize textures,
    // load models, or do other operations that you only want to do once per
    // frame.
    
	// Is this the first frame that we are rendering after starting the app?
    if (renderState.isInitialRenderCall()) {

		//For windows, we need to initialize a few more things for it to recognize all of the
		// opengl calls.
		#ifndef __APPLE__
			glewExperimental = GL_TRUE;
			GLenum err = glewInit();
			if (GLEW_OK != err)
			{
				std::cout << "Error initializing GLEW." << std::endl;
			}
		#endif     


        glEnable(GL_DEPTH_TEST);
        glClearDepth(1.0f);
        glDepthFunc(GL_LEQUAL);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glEnable(GL_MULTISAMPLE);

		// This sets the background color that is used to clear the canvas
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

		// This load shaders from disk, we do it once when the program starts up.
		reloadShaders();

        modelMesh.reset(new Model("teapot.obj", 0.035f, vec4(1.0)));
        
        turntable.reset(new TurntableManipulator(6, 1.5, 0.23));
        turntable->setCenterPosition(vec3(0, 0 ,0));

        float radius = 20.0;
        lightPosition = vec4(-1.7*radius, 0.3*radius, 1.0*radius, 1.0);
    }
}

void App::onRenderGraphicsScene(const VRGraphicsState &renderState) {
    // This routine is called once per eye/camera.  This is the place to actually
    // draw the scene.
    
	// clear the canvas and other buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    // Setup the camera with a good initial position and view direction to see the table
    glm::mat4 view = turntable->frame();
    glm::mat4 model(1.0);
    
    // Setup the projection matrix so that things are rendered in perspective
    GLfloat windowHeight = renderState.index().getValue("FramebufferHeight");
    GLfloat windowWidth = renderState.index().getValue("FramebufferWidth");
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), windowWidth / windowHeight, 0.01f, 100.0f);
    
    float w2 = windowWidth / 2.0f;
    float h2 = windowHeight / 2.0f;
    viewportMat = mat4(w2, 0.0f, 0.0f, 0.0f,
                       0.0f, h2, 0.0f, 0.0f,
                       0.0f, 0.0f, 1.0f, 0.0f,
                       w2, h2, 0.0f, 1.0f);
    
    
    // Tell opengl that we want to use the shader
    _shader.use();
    _shader.setUniform("view_mat", view);
    _shader.setUniform("projection_mat", projection);
    _shader.setUniform("model_mat", model);
    
    vec3 eyePosition = turntable->getPos();
    _shader.setUniform("eye_world", eyePosition);
    
    
    vec3 ambientReflectionCoeff = vec3(0.4, 0.4, 0.4);
    vec3 diffuseReflectionCoeff = vec3(0.8, 0.8, 0.8);
    vec3 specularReflectionCoeff = vec3(1.0, 1.0, 1.0);
    float specularExponent = 40.9;
    
    vec3 ambientLightIntensity = vec3(0.4, 0.4, 0.4);
    vec3 diffuseLightIntensity = vec3(0.6, 0.6, 0.6);
    vec3 specularLightIntensity = vec3(1.0, 1.0, 1.0);
    
    // Set shader material properties (k coeffs)
    _shader.setUniform("ambientReflectionCoeff", ambientReflectionCoeff);
    _shader.setUniform("diffuseReflectionCoeff", diffuseReflectionCoeff);
    _shader.setUniform("specularReflectionCoeff", specularReflectionCoeff);
    _shader.setUniform("specularExponent", specularExponent);
    
    // Set shader light properties (intensity and position)
    _shader.setUniform("lightPosition", lightPosition);
    _shader.setUniform("ambientLightIntensity", ambientLightIntensity);
    _shader.setUniform("diffuseLightIntensity", diffuseLightIntensity);
    _shader.setUniform("specularLightIntensity", specularLightIntensity);
    
    _shader.setUniform("viewportMatrix", viewportMat);
    
    _shader.setUniform("lineWidth", 0.25f);
    _shader.setUniform("lineColor", vec4(0.0f, 1.0f, 0.0f, 1.0f));
    
    // Draw the model
    modelMesh->draw(_shader);
    
    // Draw normals
    _normalShader.use();
    _normalShader.setUniform("normalLength", 0.2f);
    _normalShader.setUniform("view_mat", view);
    _normalShader.setUniform("projection_mat", projection);
    _normalShader.setUniform("model_mat", model);
    
    modelMesh->draw(_normalShader);
    

    
    
    
    
    // Draw the model
    modelMesh->draw(_shader);
    
    

}

void App::reloadShaders()
{
    _shader.compileShader("BlinnPhong.vert", GLSLShader::VERTEX);
    _shader.compileShader("BlinnPhong.geom", GLSLShader::GEOMETRY);
    _shader.compileShader("BlinnPhong.frag", GLSLShader::FRAGMENT);
    _shader.link();
    
    
    _normalShader.compileShader("normals.vert", GLSLShader::VERTEX);
    _normalShader.compileShader("normals.geom", GLSLShader::GEOMETRY);
    _normalShader.compileShader("normals.frag", GLSLShader::FRAGMENT);
    _normalShader.link();
}
