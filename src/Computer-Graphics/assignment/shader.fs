#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;    
    float shininess;
}; 

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float lightBoost;
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;

uniform int attentuationActivated;
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

vec4 calcLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, Material material);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    FragColor = calcLight(light, norm, FragPos, viewDir, material);
} 

vec4 calcLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, Material material)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    float d = length(light.position - fragPos);
    float attenutation = (1 / pow(d, 2)); // dL^2
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    if (attentuationActivated == 1) {
        return vec4(( 
            (ambient * attenutation) + // creates glowy effect
            (diffuse * attenutation * light.lightBoost) + // attentuation effect of over time it gets darker
            (specular * attenutation) ), 1.0); // without * attenuation the reflection is massive.
    } else {
        return vec4(( (ambient) + (diffuse) + (specular) ), 1.0); 
    }
}
