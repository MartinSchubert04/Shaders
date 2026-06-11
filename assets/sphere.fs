#version 330
  in vec3 fragPos;
  in vec3 fragNormal;

  uniform vec3 lightPos;
  uniform vec3 viewPos;

  out vec4 finalColor;

  void main() {
      vec3 lightColor  = vec3(1.0);
      vec3 objectColor = vec3(0.2, 0.5, 1.0);

      // Ambient
      vec3 ambient = 0.1 * lightColor;

      // Diffuse
      vec3 lightDir = normalize(lightPos - fragPos);
      float diff    = max(dot(fragNormal, lightDir), 0.0);
      vec3 diffuse  = diff * lightColor;

      // Specular — Blinn-Phong
      vec3 viewDir  = normalize(viewPos - fragPos);
      vec3 halfDir  = normalize(lightDir + viewDir);
      float spec    = pow(max(dot(fragNormal, halfDir), 0.0), 64.0);
      vec3 specular = 0.6 * spec * lightColor;

      finalColor = vec4((ambient + diffuse + specular) * objectColor, 1.0);
  }