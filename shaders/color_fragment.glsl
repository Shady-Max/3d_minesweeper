#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 uColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vec3 lightColor = vec3(1,1,1); /* need to add into in or uniform */

    //ambient
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;


    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse  = diff * lightColor;

    // specular
    float specularStrength = 0.5;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);


    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * uColor;

    FragColor = vec4(result, 1.0);
}