#include "FlowShader.h"

//Disclaimer: The flow shader contains code from Andrew Benson. http://abstrakt.vade.info/
// Updated to OpenGL3 by Memo Akten http://www.memo.tv

void FlowShader::setup(){

    string vertShader = getVertShader();
//    repos.load("shaders/pass.vert", "shaders/repos.frag");
    repos.setupShaderFromSource(GL_VERTEX_SHADER, vertShader);
    repos.setupShaderFromSource(GL_FRAGMENT_SHADER, getReposShader());
    repos.bindDefaults();
    repos.linkProgram();

//    flow.load("shaders/pass.vert", "shaders/flow.frag");
    flow.setupShaderFromSource(GL_VERTEX_SHADER, vertShader);
    flow.setupShaderFromSource(GL_FRAGMENT_SHADER, getFlowShader());
    flow.bindDefaults();
    flow.linkProgram();

//    blur.load("shaders/pass.vert", "shaders/blur.frag");
    blur.setupShaderFromSource(GL_VERTEX_SHADER, vertShader);
    blur.setupShaderFromSource(GL_FRAGMENT_SHADER, getBlurShader());
    blur.bindDefaults();
    blur.linkProgram();
}

///Vertex Shader Texture grabbing
string FlowShader::getVertShader(){
    string shaderProgram0 = R"(
                            varying vec2 varyingtexcoord;
                            void main()
                            {
                                gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
                                varyingtexcoord = vec2(gl_TextureMatrix[0] * gl_MultiTexCoord0);
                            }
                            )";

    string shaderProgram3 = R"(
                            #version 150

                            uniform mat4 modelViewProjectionMatrix;
                            in vec4 position;
                            in vec2 texcoord;
                            out vec2 varyingtexcoord;

                            void main(){
                               varyingtexcoord = texcoord;
                               gl_Position = modelViewProjectionMatrix * position;
                            }
                            )";
    return ofIsGLProgrammableRenderer() ? shaderProgram3 : shaderProgram0;
}

///Optical Flow modification from Andrew Benson
string FlowShader::getFlowShader(){
    string shaderHeader0 = R"(
                           varying vec2 varyingtexcoord;
                           )";
    string shaderHeader3 = R"(
                           #version 150
                           in vec2 varyingtexcoord;
//                           out vec4 gl_FragColor; // automatically declared by openframeworks
                           )";
    string shaderProgram = R"(
                           uniform sampler2DRect tex0;
                           uniform sampler2DRect tex1;
                           uniform vec2 scale;
                           uniform vec2 offset;
                           uniform float lambda;

                           vec4 getColorCoded(float x, float y,vec2 scale) {
                               vec2 xout = vec2(max(x,0.),abs(min(x,0.)))*scale.x;
                               vec2 yout = vec2(max(y,0.),abs(min(y,0.)))*scale.y;
                               float dirY = 1.0;
                               if (yout.x > yout.y)  dirY=0.90;
                               return vec4(xout.xy,max(yout.x,yout.y),dirY);
                           }

                           vec4 getGrayScale(vec4 col) {
                               float gray = dot(vec3(col.x, col.y, col.z), vec3(0.3, 0.59, 0.11));
                               return vec4(gray,gray,gray,1.0);
                           }
                           vec4 texture2DRectGray(sampler2DRect tex, vec2 coord) {
                               return getGrayScale(texture2DRect(tex, coord));
                           }

                           void main()
                           {
                               vec4 a = texture2DRectGray(tex0, varyingtexcoord);
                               vec4 b = texture2DRectGray(tex1, varyingtexcoord);
                               vec2 x1 = vec2(offset.x,0.);
                               vec2 y1 = vec2(0.,offset.y);

                               //get the difference
                               vec4 curdif = b-a;

                               //calculate the gradient
                               //for X________________
                               vec4 gradx = texture2DRectGray(tex1, varyingtexcoord+x1)-texture2DRectGray(tex1, varyingtexcoord-x1);
                               gradx += texture2DRectGray(tex0, varyingtexcoord+x1)-texture2DRectGray(tex0, varyingtexcoord-x1);


                               //for Y________________
                               vec4 grady = texture2DRectGray(tex1, varyingtexcoord+y1)-texture2DRectGray(tex1, varyingtexcoord-y1);
                               grady += texture2DRectGray(tex0, varyingtexcoord+y1)-texture2DRectGray(tex0, varyingtexcoord-y1);

                               vec4 gradmag = sqrt((gradx*gradx)+(grady*grady)+vec4(lambda));

                               vec4 vx = curdif*(gradx/gradmag);
                               vec4 vy = curdif*(grady/gradmag);

                               gl_FragColor = getColorCoded(vx.r,vy.r,scale);
                           }
                           )";
    return (ofIsGLProgrammableRenderer() ? shaderHeader3 : shaderHeader0) + shaderProgram;
}


///Reposition
string FlowShader::getReposShader(){
    string shaderHeader0 = R"(
                           varying vec2 varyingtexcoord;
                           )";
    string shaderHeader3 = R"(
                           #version 150
                           in vec2 varyingtexcoord;
//                           out vec4 gl_FragColor; // automatically declared by openframeworks
                           )";
    string shaderProgram = R"(
                           uniform vec2 amt;
                           uniform sampler2DRect tex0;
                           uniform sampler2DRect tex1;

                           vec2 get2DOff(sampler2DRect tex ,vec2 coord) {
                               vec4 col = texture2DRect(tex, coord);
                               if (col.w > 0.95) col.z *= -1.0;
                               return vec2(col.x-col.y, col.z);
                           }

                           void main()
                           {
                               vec2 coord =  get2DOff(tex1 ,varyingtexcoord)*amt+varyingtexcoord;  //relative coordinates
                               vec4 repos = texture2DRect(tex0, coord);
                               gl_FragColor = repos;
                           }
                           )";

    return (ofIsGLProgrammableRenderer() ? shaderHeader3 : shaderHeader0) + shaderProgram;
}

//Vertical and Horizontal Blur
string FlowShader::getBlurShader(){
    string shaderHeader0 = R"(
                           varying vec2 varyingtexcoord;
                           )";
    string shaderHeader3 = R"(
                           #version 150
                           in vec2 varyingtexcoord;
//                           out vec4 gl_FragColor; // automatically declared by openframeworks
                           )";
    string shaderProgram = R"(
                           uniform sampler2DRect texture;
                           uniform vec2 texOffset;
                           uniform float blurSize;
                           uniform float horizontalPass; // 0 or 1 to indicate vertical or horizontal pass
                           uniform float sigma;        // The sigma value for the gaussian function: higher value means more blur
                           // A good value for 9x9 is around 3 to 5
                           // A good value for 7x7 is around 2.5 to 4
                           // A good value for 5x5 is around 2 to 3.5
                           // ... play around with this based on what you need :)

                           const float pi = 3.14159265;

                           vec4 get2DOff(sampler2DRect tex ,vec2 coord) {
                               vec4 col = texture2DRect(tex, coord);
                               if (col.w > 0.95)  col.z *= -1.0;
                               return vec4(col.y-col.x, col.z, 1.0, 1.0);
                           }


                           vec4 getColorCoded(float x, float y,vec2 scale) {
                               vec2 xout = vec2(max(x,0.),abs(min(x,0.)))*scale.x;
                               vec2 yout = vec2(max(y,0.),abs(min(y,0.)))*scale.y;
                               float dirY = 1.0;
                               if (yout.x > yout.y)  dirY=0.90;
                               return vec4(xout.yx,max(yout.x,yout.y),dirY);
                           }

                           void main() {
                               float numBlurPixelsPerSide = float(blurSize / 2.0);

                               vec2 blurMultiplyVec = 0.0 < horizontalPass ? vec2(1.0, 0.0) : vec2(0.0, 1.0);

                               // Incremental Gaussian Coefficent Calculation (See GPU Gems 3 pp. 877 - 889)
                               vec3 incrementalGaussian;
                               incrementalGaussian.x = 1.0 / (sqrt(2.0 * pi) * sigma);
                               incrementalGaussian.y = exp(-0.5 / (sigma * sigma));
                               incrementalGaussian.z = incrementalGaussian.y * incrementalGaussian.y;

                               vec4 avgValue = vec4(0.0, 0.0, 0.0, 0.0);
                               float coefficientSum = 0.0;

                               // Take the central sample first...
                               avgValue += get2DOff(texture, varyingtexcoord.st) * incrementalGaussian.x;
                               coefficientSum += incrementalGaussian.x;
                               incrementalGaussian.xy *= incrementalGaussian.yz;

                               // Go through the remaining 8 vertical samples (4 on each side of the center)

                               for (float i = 1.0; i <= numBlurPixelsPerSide; i++) {
                                   avgValue += get2DOff(texture, varyingtexcoord.st - i * texOffset *
                                                        blurMultiplyVec) * incrementalGaussian.x;
                                   avgValue += get2DOff(texture, varyingtexcoord.st + i * texOffset *
                                                        blurMultiplyVec) * incrementalGaussian.x;
                                   coefficientSum += 2.0 * incrementalGaussian.x;
                                   incrementalGaussian.xy *= incrementalGaussian.yz;
                               }


                               vec4 finColor = avgValue / coefficientSum;


                               gl_FragColor = getColorCoded(finColor.x, finColor.y,vec2(1.0,1.0));
                           }
                           )";

    return (ofIsGLProgrammableRenderer() ? shaderHeader3 : shaderHeader0) + shaderProgram;
}
