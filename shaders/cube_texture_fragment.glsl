#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D uTexture;
uniform samplerCube skybox;     // Cubemap for reflections

uniform vec3 uLightPos;
uniform vec3 uViewPos;

struct LightingParams {
    float ambient;
    float diffuse;
    float specular;
    float shininess;
    float reflection;
};
uniform LightingParams lp;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(uLightPos - FragPos);

    // diffuse lighting
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = vec3(lp.diffuse) * diff;
    vec3 ambient = vec3(lp.ambient);

    // view direction
    vec3 viewDir = normalize(uViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    vec3 reflectedColor = texture(skybox, reflectDir).rgb;

    vec3 albedo = texture(uTexture, TexCoord).rgb;

    // specular lighting
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), lp.shininess);
    vec3 specular = vec3(lp.specular) * spec;


    vec3 lighting = ambient + diffuse + specular;
    vec3 finalColor = mix(albedo * lighting, reflectedColor, lp.reflection);

    FragColor = vec4(finalColor, 1.0);
}