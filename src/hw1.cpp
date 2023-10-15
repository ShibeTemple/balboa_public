#include "hw1.h"
#include "hw1_scenes.h"


using namespace hw1;

Image3 hw_1_1(const std::vector<std::string> &params) {
    // Homework 1.1: render a circle at the specified
    // position, with the specified radius and color.

    // center
    // radius
    // color

    // Core image to return
    Image3 img(640 /* width */, 480 /* height */);

    // Set default values for paramaters 
    Vector2 center = Vector2{img.width / 2 + Real(0.5), img.height / 2 + Real(0.5)};
    Real radius = 100.0;
    Vector3 color = Vector3{1.0, 0.5, 0.5};

    // Gather passed paramaters
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-center") {
            Real x = std::stof(params[++i]);
            Real y = std::stof(params[++i]);
            center = Vector2{x, y};
        } else if (params[i] == "-radius") {
            radius = std::stof(params[++i]);
        } else if (params[i] == "-color") {
            Real r = std::stof(params[++i]);
            Real g = std::stof(params[++i]);
            Real b = std::stof(params[++i]);
            color = Vector3{r, g, b};
        }
    }

    // Draw pixels on core image
    //
    // for each pixel
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            // y
            // x
            // color
            //
            // This implementation can be improved by only considering pixels within circle boundary box.
            // However, per instructions: "Grading. We will compare your outputs to our reference solutions."
            // I do not want to mess up the implementation of that at the sake of a grade.

            // base_color
            // updated from {1,1,1} to {0.5,0.5,0.5} per Piazza Q@12
            Vector3 base_color = Vector3{ 0.5, 0.5, 0.5 };

            // Calculate distance from center of circle to current point for every point
            // https://math.stackexchange.com/a/198769
            Real pythag_a = pow((center.x - x), 2);
            Real pythag_b = pow((center.y - y), 2);
            Real distance_from_center = sqrt(( pythag_a + pythag_b ));

            // if pixel lands on circle
            // 
            // started with <= but moved to < per Figure 5 & Piazza Q@14. 
            // Difference is drawing 1 extra pixel on circle boundary. See Piazza Q@14 for details.
            if (distance_from_center < radius) {
                img(x, y) = color;
            } else {
                img(x, y) = base_color;
            }
            
        }
    }
    return img;
}

Image3 hw_1_2(const std::vector<std::string> &params) {
    // Homework 1.2: render multiple circles
    // Default, blank return
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    int scene_id = std::stoi(params[0]);
    const CircleScene &scene = hw1_2_scenes[scene_id];

    Image3 img(scene.resolution.x, scene.resolution.y);

    // draw background
    // maybe could be time complexity reduced?
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = scene.background;
        }
    }


    // for each pixel
    //
    // circles are ordered from the farthest to the closest.
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            // y
            // x
            // color
            
            // size_t https://en.cppreference.com/w/c/types/size_t
            for (size_t i = 0; i < scene.objects.size(); ++i) {
                // Access the circle object within the loop using circle.center, circle.radius, circle.color
                const Circle& circle = scene.objects[i];

                // Calculate distance from center of circle to current point for every point
                // https://math.stackexchange.com/a/198769
                Real pythag_a = pow((circle.center.x - x), 2);
                Real pythag_b = pow((circle.center.y - y), 2);
                Real distance_from_center = sqrt((pythag_a + pythag_b));

                // if pixel lands on circle
                // 
                // started with <= but moved to < per Figure 5 & Piazza Q@14. 
                // Difference is drawing 1 extra pixel on circle boundary. See Piazza Q@14 for details.
                if (distance_from_center < circle.radius) {
                    img(x, y) = circle.color;
                }
            }


        }
    }
    return img;
}

Image3 hw_1_3(const std::vector<std::string> &params) {
    // Homework 1.3: render multiple shapes
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{1, 1, 1};
        }
    }
    return img;
}

Image3 hw_1_4(const std::vector<std::string> &params) {
    // Homework 1.4: render transformed shapes
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{1, 1, 1};
        }
    }
    return img;
}

Image3 hw_1_5(const std::vector<std::string> &params) {
    // Homework 1.5: antialiasing
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{1, 1, 1};
        }
    }
    return img;
}

Image3 hw_1_6(const std::vector<std::string> &params) {
    // Homework 1.6: alpha blending
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{1, 1, 1};
        }
    }
    return img;
}
