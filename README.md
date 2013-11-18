ofxMIOFlowGLSL
==============

Optical Flow GPU Version - based upon Andrew Benson's solution
This is a simple GLSL based solution for calculating optical flow. The main benefit is to calculate the flow on GPU and then 
keep the results within a raw texture to forward it for further Shader based processing.

Advantages
======================
Hence it is possible to define Ping pong shaders with particles or complex objects in Geometry shader who are feeded by the flow texture.
Another advantage of this GPU solution is its speed. Hence it is appropriate for detecting flow in Full HD images or on top of depth data.


Installation
======================
just copy the content of the zip file in your OF addon path. Link it in your project and you are ready to go.


HOW TO
======================
try the example file you will see that everything is pretty straight forward and doesnt require more then

1) setup(w,h) - to setup the context in (w)idth and (h)eight

2) update(myCamTexture) - your current texture

3) getFlowBlurTexture() - to access the texture of your optical flow

you can also simply preview your results by the use of drawReposition(0,0);


HOW TO USE IN SHADER
======================
The direction of the current flow per pixel is coded in each fragment color. If you pass the flow texture to the next 
shader then youll have to lookup the flow offset coordinates in 2d space just like that:

	vec2 get2DOff(sampler2DRect tex ,vec2 coord) {
		vec4 col = texture2DRect(tex, coord);
		if (col.w >0.95)  col.z=col.z*-1;
		return vec2(-1*(col.y-col.x),col.z);//,1,1);
	}

the return 2 dimensional vector is your offset in 2d Space...play with it...


FUTURE WORK
======================
im going to try my best to add scene flow functionalities to the current algorithm. Hence it would be possible to detect
the flow of pointclouds in 3D Space. My current idea is to measure the depth values of the pixels in refernce to its 2d offset.
Hence it would be some kind of 2 1/2 scene flow ;). i do believe that this could be very easily achieved and very fast. If you
have alternative ideas - please let me know!





ENJOY
