#include <vector>

struct quat {};
struct vec3f {};
struct Vao {};


struct Actor
{
    ActorDef definition;
    CompiledPose pose;

    vec3f position;
    quat rotation;
};


struct ActorDef
{
    Vao mesh;
    std::vector<BoneDef> bones;
};


struct BoneDef
{
    vec3f position;
    quat rotation;
    int parent;
};
    

struct Animation
{
    std::vector<vec3f> positions;
};


// Poses can be interpolated
struct Pose
{
    std::vector<vec3f> positions;
    std::vector<quat> rotations;
    std::vector<float> times;

    CompiledPose Compile();
};


// CompoledPose are created from Pose and are rendered
struct CompiledPose
{
    std::vector<mat4f> transforms;
};
