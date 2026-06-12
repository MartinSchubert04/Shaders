#version 330
  in vec3 fragPos;
  in vec3 fragNormal;
  
  uniform vec4 fragColor;
  uniform vec3 lightPos;
  uniform vec3 lightPos2;
  uniform vec3 viewPos;

  out vec4 finalColor;

  vec3 calcLight(vec3 lightPos, vec3 viewDir, vec3 normal, vec3 pos) {
      vec3 lightDir = normalize(lightPos - pos);
      float diff    = max(dot(normal, lightDir), 0.0);
      vec3 halfDir  = normalize(lightDir + viewDir);
      float spec    = pow(max(dot(normal, halfDir), 0.0), 64.0);
      return diff * vec3(1.0) + 0.6 * spec * vec3(1.0);
  }

  void main() {
      vec3 objectColor = fragColor.rgb;

      vec3 viewDir = normalize(viewPos - fragPos);
      vec3 ambient = 0.1 * vec3(1.0);

      vec3 light = calcLight(lightPos,  viewDir, fragNormal, fragPos)
                 + calcLight(lightPos2, viewDir, fragNormal, fragPos);

    //   finalColor = vec4((ambient + light) * objectColor, 1.0);
      finalColor = vec4(objectColor, 1.0);
  }