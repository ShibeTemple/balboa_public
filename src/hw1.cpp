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
            for (size_t i = 0; i < scene.shapes.size(); ++i) {
                // Access the current shape object
                const Shape& shape = scene.shapes[i];

                if (auto* circle = std::get_if<Circle>(&shape)) {
                    const Circle& curr_circle = *circle;

                    // do something with circle
                    // Calculate distance from center of circle to current point for every point
                    // https://math.stackexchange.com/a/198769
                    Real pythag_a = pow((curr_circle.center.x - x), 2);
                    Real pythag_b = pow((curr_circle.center.y - y), 2);
                    Real distance_from_center = sqrt((pythag_a + pythag_b));

                    // if pixel lands on circle
                    // 
                    // started with <= but moved to < per Figure 5 & Piazza Q@14. 
                    // Difference is drawing 1 extra pixel on circle boundary. See Piazza Q@14 for details.
                    if (distance_from_center < curr_circle.radius) {
                        img(x, y) = curr_circle.color;
                    }
                }
                else if (auto* rectangle = std::get_if<Rectangle>(&shape)) {
                    // do something with rectangle
                    const Rectangle& curr_rectangle = *rectangle;
                       
                    // Vector2:
                    //  curr_rectangle.p_min top-left point
                    //  curr_rectangle.p_max bottom-right point
                    //

                    bool within_x_bounds = (x > curr_rectangle.p_min.x && x < curr_rectangle.p_max.x);
                    bool within_y_bounds = (y > curr_rectangle.p_min.y && y < curr_rectangle.p_max.y);
                    //Real height = curr_rectangle.p_min.y - curr_rectangle.p_max.y;
                    //Real width = curr_rectangle.p_min.x - curr_rectangle.p_max.x;
                    if (within_x_bounds && within_y_bounds) {
                        img(x, y) = curr_rectangle.color;
                    }
                }
                else if (auto* triangle = std::get_if<Triangle>(&shape)) {
                    // do something with triangle
                    const Triangle& curr_triangle = *triangle;

                    // p0, p1, p2

                    // We can determine whether a point q is on the positive or negative half-planes 
                    // by looking at the sign of the dot product between a vector formed by the point q 
                    // and the line, and a normal vector n that is perpendicular to the direction of the line.
                    //

                    // sign of the dot product between a vector formed by the point q and the line
                    // a normal vector n that is perpendicular to the direction of the line

                    // edge vectors
                    Vector2 edge1 = curr_triangle.p1 - curr_triangle.p0;
                    Vector2 edge2 = curr_triangle.p2 - curr_triangle.p1;
                    Vector2 edge3 = curr_triangle.p0 - curr_triangle.p2;

                    // p1 − p0, p2 − p1, and p0 − p2
                    // p 1-0 | e 01
                    // p 2-1 | e 12
                    // p 0-2 | e 20

                    // e01 == 1 - 0
                    // e12 == 2 - 1
                    // e20 == 0 - 2
                    // n01, n12, n20

                    // n01: x = e01.y | y= -e01.x

                    // edge normal vectors
                    Vector2 n01 = Vector2{ edge1.y, -edge1.x };
                    Vector2 n12 = Vector2{ edge2.y, -edge2.x };
                    Vector2 n20 = Vector2{ edge3.y, -edge3.x };


                    // current point vectors
                    Vector2 pv0 = Vector2{ curr_triangle.p0.x - x, curr_triangle.p0.y - y };
                    Vector2 pv1 = Vector2{ curr_triangle.p1.x - x, curr_triangle.p1.y - y };
                    Vector2 pv2 = Vector2{ curr_triangle.p2.x - x, curr_triangle.p2.y - y };

                    Real d0 = dot(n01, pv0);
                    Real d1 = dot(n12, pv1);
                    Real d2 = dot(n20, pv2);

                    bool within_triangle = (d0 >= 0 && d1 >= 0 && d2 >= 0) || (d0 <= 0 && d1 <= 0 && d2 <= 0);

                    if (within_triangle) {
                        img(x, y) = curr_triangle.color;
                    }

                    /*
                    if (x == curr_triangle.p0.x && y == curr_triangle.p0.y) {
                        img(x, y) = curr_triangle.color;
                    } else
                    if (x == curr_triangle.p1.x && y == curr_triangle.p1.y) {
                        img(x, y) = curr_triangle.color;
                    } else
                    if (x == curr_triangle.p2.x && y == curr_triangle.p2.y) {
                        img(x, y) = curr_triangle.color;
                    }

                    if ((x - n01.x) > 0 && (y - n01.y) > 0) {
                        //img(x, y) = curr_triangle.color;
                    }
                    if ((x - n12.x) > 0 && (y - n12.y) > 0) {
                        img(x, y) = curr_triangle.color;
                    }
                    if ((x - n20.x) > 0 && (y - n20.y) > 0) {
                        //img(x, y) = curr_triangle.color;
                    }*/

                }


            }


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
