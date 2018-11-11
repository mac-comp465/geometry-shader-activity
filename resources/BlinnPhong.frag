#version 330

// Fragment shader

// Material Properties
uniform vec3 ambientReflectionCoeff;
uniform vec3 diffuseReflectionCoeff;
uniform vec3 specularReflectionCoeff;
uniform float specularExponent;

// Light Properties
uniform vec3 ambientLightIntensity;
uniform vec3 diffuseLightIntensity;
uniform vec3 specularLightIntensity;

uniform vec4 lightPosition;

uniform vec3 eye_world;

uniform float lineWidth;
uniform vec4 lineColor;

// These get passed in from the vertex shader and are interpolated (varying) properties
// change for each pixel across the triangle:
in vec4 interpSurfPosition;
in vec3 interpSurfNormal;
noperspective in vec3 interpEdgeDistance;

out vec4 fragColor;


vec3 phongModel() {
	// interpNormal is linearly interpolated across the surface, but it
	// is *NOT* guaranteed to be of unit length, which can cause
	// unappealing lighting artifacts.  This normalizes the vector to be
	// of unit length.
	vec3 N = normalize(interpSurfNormal);

	vec3 L = normalize(lightPosition - interpSurfPosition).xyz;


    // Start with black and then add lighting to the final color as we calculate it
	vec3 finalColor = vec3(0.0, 0.0, 0.0);


	// AMBIENT:
	finalColor += ambientLightIntensity * ambientReflectionCoeff;


	// DIFFUSE:
    float NdotL = clamp(dot(N,L), 0.0, 1.0);
    finalColor += diffuseLightIntensity * diffuseReflectionCoeff * NdotL;
	


	// SPECULAR:
	if (NdotL > 0.0) {
        vec3 V = normalize(eye_world - interpSurfPosition.xyz);
        
        vec3 H = normalize(L + V);
		float NdotH = max(0.0, dot(H, N));

		float specIntensity = clamp(pow(NdotH, specularExponent), 0.0, 1.0);

        finalColor += specularLightIntensity * specularReflectionCoeff * specIntensity;
	}
    
    return finalColor;
}

void main() {
    
    // The following code is based on an example from the OpenGL 4 Shading Language Cookbook by David Wolff


    // Shaded surface color based on the phong model
    vec4 color = vec4(phongModel(), 1.0);
    
    // Find the smallest distance to an edge of the triangle
    float d = min(interpEdgeDistance.x, min(interpEdgeDistance.y, interpEdgeDistance.z));
    
    // Determine the mix factor with the line color
    float mixVal = smoothstep(lineWidth - 1.0, lineWidth + 1.0, d);
    
    // Mix the surface color with the line color
    fragColor = mix(lineColor, color, mixVal);
}
