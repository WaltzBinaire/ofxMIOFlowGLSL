ofxMIOFlowGLSL
==============

Optical Flow GPU Version - based upon Andrew Benson's solution


This is a simple GLSL based solution for calculating optical flow. The main benefit is to calculate the flow on GPU and then 
keep the results within a raw texture to forward it for further Shader based processing.

Hence it is possible to define Ping pong shaders with particles or complex objects in Geometry shader who are feeded by the flow texture.

Another advantage of this GPU solution its speed. Hence it is appropriate for detecting flow in Full HD images or on to
