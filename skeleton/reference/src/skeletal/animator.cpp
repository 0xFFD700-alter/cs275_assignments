#include "skeletal/animator.hpp"
#include "gltf/tinygltf_helper.h"

bool SkeletalAnimator::
loadFromTinyGLTF(
    const tinygltf::Model& mdl,
    const Skeleton& skel, 
    std::string& warn, 
    std::string& err
) {
    if (mdl.animations.size() == 0) {
        err = "No skeletal animation data in file.";
        return false;
    }

    const tinygltf::Animation& gltfAnim = mdl.animations[0];

    //TimeGetter will grab the timestamp of a given keyframe.
    //KeyGetter will grab the actual keyframe data.
    tinygltf_DataGetter timeGetter, keyGetter;

    float maxTime = 0.0f; 

    std::set<float> timeset;
    keyframe_states.resize(mdl.skins[0].joints.size());
    for (size_t i = 0; i < gltfAnim.channels.size(); ++i)
    {
        const tinygltf::AnimationChannel& channel = gltfAnim.channels[i];
        const tinygltf::AnimationSampler& sampler = gltfAnim.samplers[channel.sampler];

        // `channel.target_node` tells you the index of nodes in gltf file is using the animation

        timeGetter = tinygltf_buildDataGetter(mdl, sampler.input);
        keyGetter = tinygltf_buildDataGetter(mdl, sampler.output);

        if (timeGetter.len == 0 || keyGetter.len == 0)
            continue;
        
        if (timeGetter.elementSize != sizeof(float)) {
            err = "The time of the animation per keyframe should be a float number";
            return false;
        }
        if (keyGetter.elementSize != 4 * sizeof(float)) {
            err = "The data of the animation per keyframe should be a quaternion consists of 4 floats";
            return false;
        }
        
        // Normally we'd use templates to keep this sort of thing cleaned up.
        // Unfortunately, that whole difference in quaternion layout throws a
        // wrinkle into that - we can't just memcpy into a raw glm::quat.
        // Rather than correct after the fact and swap X/W coordinates,
        // we'll just do this.
        
        // We only care about joint rotation.
        int totalFrames = static_cast<int>(keyGetter.len);
        if (channel.target_path == "rotation") {
            int count = 0;
            for (size_t k = 0; k < timeGetter.len && k < keyGetter.len; ++k) {
                float fTime;
                float x, y, z, w;

                memcpy(&fTime, &timeGetter.data[k * timeGetter.stride], sizeof(float));

                if (fTime > maxTime)
                    maxTime = fTime;

                memcpy(&x, &keyGetter.data[k * keyGetter.stride + 0 * sizeof(float)], sizeof(float));
                memcpy(&y, &keyGetter.data[k * keyGetter.stride + 1 * sizeof(float)], sizeof(float));
                memcpy(&z, &keyGetter.data[k * keyGetter.stride + 2 * sizeof(float)], sizeof(float));
                memcpy(&w, &keyGetter.data[k * keyGetter.stride + 3 * sizeof(float)], sizeof(float));

               
                KeyframeState current_frame;
                current_frame.timestamp = fTime;
                current_frame.rotationData = glm::quat(w, x, y, z);
                keyframe_states[channel.target_node].push_back(current_frame);
                timeset.insert(fTime);
               

                count++;
                /** These property might be helpful
                 * channel.target_node
                 * fTime
                 * quat: x, y, z, w
                 * */
            }
        }
    }

    // times = std::vector<float>(timeset.begin(), timeset.end());
    // std::sort(times.begin(), times.end());

    // size_t j = 0;
    // KeyframeState frame;
    // float intrplT = 0.0f;

    // for (size_t i = 0; i < keyframe_states.size(); ++i) {
    //     if (keyframe_states[i].empty() || keyframe_states[i].size() == times.size()) {
    //         continue;
    //     }
    //     for (j = 0;; ++j) {
    //         if (times[j] == keyframe_states[i][j].timestamp) {
    //             break;
    //         } else {
    //             frame.timestamp = times[j];
    //             frame.rotationData = skel.joints[i].nodeRotate;
    //             keyframe_states[i].insert(keyframe_states[i].begin() + j, frame);
    //         }
    //     }
    //     for (;; ++j) {
    //         if (j == keyframe_states[i].size()) {
    //             break;
    //         }
    //         else if (times[j] != keyframe_states[i][j].timestamp) {
    //             intrplT = (times[j] - keyframe_states[i][j - 1].timestamp) / (keyframe_states[i][j].timestamp - keyframe_states[i][j - 1].timestamp);
    //             frame.timestamp = times[j];
    //             frame.rotationData = keyframe_states[i][j - 1].rotationData * (1 - intrplT) + keyframe_states[i][j].rotationData * intrplT;
    //             keyframe_states[i].insert(keyframe_states[i].begin() + j, frame);
    //         }
    //     }
    //     for (; j < times.size(); ++j) {
    //         frame.timestamp = times[j];
    //         frame.rotationData = keyframe_states[i][j - 1].rotationData;
    //         keyframe_states[i].insert(keyframe_states[i].begin() + j, frame);
    //     }
    // }
    
    return true; 
}