#version 330

// Geometry shader

// The following code is based on an example from the OpenGL 4 Shading Language Cookbook by David Wolff
// and a whitepaper by Nvidia from 2007 (Solid Wireframe, Nvidia Whitepape WP-03014-001_v01)


// Geometry Shaders are run once for each geometric primitive (usually a triangle)
// You must start by defining the kind of primitive to expect.
// The primitive can be one of the following: points, lines, lines_adjacency, triangles, triangles_adjacency

layout (triangles) in;

// You also need to specify the type of primitive that the geometry shader will produce and the
// maximum number of vertices you will output.
// Possible options are: points, line_strip, triangle_strip
// Even though we are outputing a triangle strip, we will only do a triangle at a time, so 3 vertices.
layout (triangle_strip, max_vertices = 3) out;

//Because we operate on a primitive at a time, the vertex positions and normals are arrays.
in vec4 position[];
in vec3 normal[];

// OUTPUT: to the fragment shader

out vec4 interpSurfPosition;
out vec3 interpSurfNormal;

// This holds the shortest distance to each edge of the triangle from the vertex
noperspective out vec3 interpEdgeDistance;

uniform mat4 viewportMatrix;

void main(void)
{
    // Convert points into pixel coordinates so we can do the length in 2D
    vec3 p0 = vec3(viewportMatrix * (gl_in[0].gl_Position / gl_in[0].gl_Position.w));
    vec3 p1 = vec3(viewportMatrix * (gl_in[1].gl_Position / gl_in[1].gl_Position.w));
    vec3 p2 = vec3(viewportMatrix * (gl_in[2].gl_Position / gl_in[2].gl_Position.w));
    
    
    
    //                   a     b
    //                   *-----*
    //                    \    |
    //                     \   |
    //                      \  |
    //                       \ |
    //                        \|
    //                         *
    //                         c


    
	// Find the altitudes (ha, hb, and hc)
    // ha is the shortest distance from a to the side b-c, etc.
    float a = length(p1 - p2);
    float b = length(p2 - p0);
    float c = length(p1 - p0);
    float alpha = acos( (b*b + c*c - a*a) / (2.0*b*c) );
    float beta = acos( (a*a + c*c - b*b) / (2.0*a*c) );
    float ha = abs( c * sin( beta ) );
    float hb = abs( c * sin( alpha ) );
    float hc = abs( b * sin( alpha ) );
    
    interpEdgeDistance = vec3( ha, 0, 0 );
    interpSurfNormal = normal[0];
    interpSurfPosition = position[0];
    gl_Position = gl_in[0].gl_Position; // Just like the vertex shader you need to tell opengl where the vertex is located.
    // This sends a vertex down the pipeline
    EmitVertex();
    
    interpEdgeDistance = vec3( 0, hb, 0 );
    interpSurfNormal = normal[1];
    interpSurfPosition = position[1];
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    
    interpEdgeDistance = vec3( 0, 0, hc );
    interpSurfNormal = normal[2];
    interpSurfPosition = position[2];
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
    
    // Specifies the end of a primitive
    EndPrimitive();
}
