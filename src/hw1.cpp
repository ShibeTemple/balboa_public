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

Vector3 object_space(Real x, Real y, Matrix3x3 transform) {
    return inverse(transform) * Vector3{ x, y, 1.0 };
}

Image3 hw_1_4(const std::vector<std::string> &params) {
    // Homework 1.4: render transformed shapes
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);

    // for each pixel
    //
    // circles are ordered from the farthest to the closest.
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            // y
            // x
            // color

            // default, draw background.
            img(x, y) = scene.background;

            // size_t https://en.cppreference.com/w/c/types/size_t
            for (size_t i = 0; i < scene.shapes.size(); ++i) {
                // Access the current shape object
                const Shape& shape = scene.shapes[i];

                if (auto* circle = std::get_if<Circle>(&shape)) {
                    const Circle& curr_circle = *circle;
                    // do something with circle

                    // get object space
                    // translate given coords to object space of shape
                    Vector3 current_object_space = object_space(x, y, curr_circle.transform);
                    
                    // Calculate distance from center of circle to current point for every point
                    // https://math.stackexchange.com/a/198769
                    Real pythag_a = pow((curr_circle.center.x - current_object_space.x), 2);
                    Real pythag_b = pow((curr_circle.center.y - current_object_space.y), 2);
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
                    
                    // get object space
                    // translate given coords to object space of shape
                    Vector3 current_object_space = object_space(x, y, rectangle->transform);

                    bool within_x_bounds = (current_object_space.x > curr_rectangle.p_min.x && 
                        current_object_space.x < curr_rectangle.p_max.x);
                    bool within_y_bounds = (current_object_space.y > curr_rectangle.p_min.y && 
                        current_object_space.y < curr_rectangle.p_max.y);
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

                    // get object space
                    // translate given coords to object space of shape
                    Vector3 current_object_space = object_space(x, y, triangle->transform);

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
                    Vector2 pv0 = Vector2{ curr_triangle.p0.x - current_object_space.x, curr_triangle.p0.y - current_object_space.y };
                    Vector2 pv1 = Vector2{ curr_triangle.p1.x - current_object_space.x, curr_triangle.p1.y - current_object_space.y };
                    Vector2 pv2 = Vector2{ curr_triangle.p2.x - current_object_space.x, curr_triangle.p2.y - current_object_space.y };

                    Real d0 = dot(n01, pv0);
                    Real d1 = dot(n12, pv1);
                    Real d2 = dot(n20, pv2);

                    bool within_triangle = (d0 >= 0 && d1 >= 0 && d2 >= 0) || (d0 <= 0 && d1 <= 0 && d2 <= 0);

                    if (within_triangle) {
                        img(x, y) = curr_triangle.color;
                    }
                }
            }
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

    Real SSAA_factor = 4;
    // for each pixel
    //
    // circles are ordered from the farthest to the closest.
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            // y
            // x
            // color

            //std::cout << "xy " << x << y << "\n";

            std::vector<Vector3> sub_pixels;
            Vector3 current_pixel;

            for (Real ssY = y; ssY < y + 1; ssY += (1.0 / SSAA_factor)) {
                for (Real ssX = x; ssX < x + 1; ssX += (1.0 / SSAA_factor)) {

                    //std::cout << "ss " << ssX << ssY << "\n";
                    current_pixel = scene.background;
                    
                    // size_t https://en.cppreference.com/w/c/types/size_t
                    for (size_t i = 0; i < scene.shapes.size(); ++i) {
                        // Access the current shape object
                        const Shape& shape = scene.shapes[i];

                        if (auto* circle = std::get_if<Circle>(&shape)) {
                            const Circle& curr_circle = *circle;
                            // do something with circle

                            // get object space
                            // translate given coords to object space of shape
                            Vector3 current_object_space = object_space(ssX, ssY, curr_circle.transform);

                            // Calculate distance from center of circle to current point for every point
                            // https://math.stackexchange.com/a/198769
                            Real pythag_a = pow((curr_circle.center.x - current_object_space.x), 2);
                            Real pythag_b = pow((curr_circle.center.y - current_object_space.y), 2);
                            Real distance_from_center = sqrt((pythag_a + pythag_b));

                            // if pixel lands on circle
                            // 
                            // started with <= but moved to < per Figure 5 & Piazza Q@14. 
                            // Difference is drawing 1 extra pixel on circle boundary. See Piazza Q@14 for details.
                            if (distance_from_center < curr_circle.radius) {
                                current_pixel = curr_circle.color;
                            }
                        }
                        else if (auto* rectangle = std::get_if<Rectangle>(&shape)) {
                            const Rectangle& curr_rectangle = *rectangle;
                            // do something with rectangle

                            // Vector2:
                            //  curr_rectangle.p_min top-left point
                            //  curr_rectangle.p_max bottom-right point

                            // get object space
                            // translate given coords to object space of shape
                            Vector3 current_object_space = object_space(ssX, ssY, rectangle->transform);

                            bool within_x_bounds = (current_object_space.x > curr_rectangle.p_min.x &&
                                current_object_space.x < curr_rectangle.p_max.x);
                            bool within_y_bounds = (current_object_space.y > curr_rectangle.p_min.y &&
                                current_object_space.y < curr_rectangle.p_max.y);
                            //Real height = curr_rectangle.p_min.y - curr_rectangle.p_max.y;
                            //Real width = curr_rectangle.p_min.x - curr_rectangle.p_max.x;
                            if (within_x_bounds && within_y_bounds) {
                                current_pixel = curr_rectangle.color;
                            }
                        }
                        else if (auto* triangle = std::get_if<Triangle>(&shape)) {
                            const Triangle& curr_triangle = *triangle;
                            // do something with triangle

                            // p0, p1, p2

                            // We can determine whether a point q is on the positive or negative half-planes 
                            // by looking at the sign of the dot product between a vector formed by the point q 
                            // and the line, and a normal vector n that is perpendicular to the direction of the line.
                            //

                            // sign of the dot product between a vector formed by the point q and the line
                            // a normal vector n that is perpendicular to the direction of the line

                            // get object space
                            // translate given coords to object space of shape
                            Vector3 current_object_space = object_space(ssX, ssY, triangle->transform);

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
                            Vector2 pv0 = Vector2{ curr_triangle.p0.x - current_object_space.x, curr_triangle.p0.y - current_object_space.y };
                            Vector2 pv1 = Vector2{ curr_triangle.p1.x - current_object_space.x, curr_triangle.p1.y - current_object_space.y };
                            Vector2 pv2 = Vector2{ curr_triangle.p2.x - current_object_space.x, curr_triangle.p2.y - current_object_space.y };

                            Real d0 = dot(n01, pv0);
                            Real d1 = dot(n12, pv1);
                            Real d2 = dot(n20, pv2);

                            bool within_triangle = (d0 >= 0 && d1 >= 0 && d2 >= 0) || (d0 <= 0 && d1 <= 0 && d2 <= 0);

                            if (within_triangle) {
                                current_pixel = curr_triangle.color;
                            }
                        }


                    }

                    // save current pixel to the SSAA array
                    // https://cplusplus.com/reference/vector/vector/push_back/
                    sub_pixels.push_back(current_pixel);
                    //std::cout << "hello\n";

                }
            }

            // average the current pixel's (x,y) SSAA array (scale of SSAA_factor).
            // r g b
            Vector3 average_pixel = Vector3{0,0,0};
            //std::cout << size(sub_pixels) << "=======================================================================\n";
            for (size_t k = 0; k < size(sub_pixels); k++) {
                average_pixel.x += sub_pixels[k].x;
                average_pixel.y += sub_pixels[k].y;
                average_pixel.z += sub_pixels[k].z;
            }
            //std::cout << "done\n\n";

            average_pixel.x = average_pixel.x / size(sub_pixels);
            average_pixel.y = average_pixel.y / size(sub_pixels);
            average_pixel.z = average_pixel.z / size(sub_pixels);

            img(x, y) = average_pixel;

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


    Real SSAA_factor = 4;
    // for each pixel
    //
    // circles are ordered from the farthest to the closest.
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            // y
            // x
            // color

            //std::cout << "xy " << x << y << "\n";

            std::vector<Vector3> sub_pixels;
            Vector3 current_pixel;

            for (Real ssY = y; ssY < y + 1; ssY += (1.0 / SSAA_factor)) {
                for (Real ssX = x; ssX < x + 1; ssX += (1.0 / SSAA_factor)) {
                    
                    //Real num_layers = 0;
                    //std::vector<Vector3> alpha_layers;
                    //std::vector<Real> alphas;
                    current_pixel = scene.background;

                    // C = Alpha * ShapeColor + Background-ShapeAlpha
                    // C = a*S + (1 - a)*Background
                    // C = a0*S0 + a1(1 - a0)S1 + (1 - a0)(1 - a1)*Background
                    // C = Last Shape + Previous Shape + Background
                    // C = Last Shape + Previous Shape(s) + Background

                    // size_t https://en.cppreference.com/w/c/types/size_t
                    for (size_t i = 0; i < scene.shapes.size(); ++i) {
                        // Access the current shape object
                        const Shape& shape = scene.shapes[i];

                        if (auto* circle = std::get_if<Circle>(&shape)) {
                            const Circle& curr_circle = *circle;
                            // do something with circle

                            // get object space
                            // translate given coords to object space of shape
                            Vector3 current_object_space = object_space(ssX, ssY, curr_circle.transform);

                            // Calculate distance from center of circle to current point for every point
                            // https://math.stackexchange.com/a/198769
                            Real pythag_a = pow((curr_circle.center.x - current_object_space.x), 2);
                            Real pythag_b = pow((curr_circle.center.y - current_object_space.y), 2);
                            Real distance_from_center = sqrt((pythag_a + pythag_b));

                            // if pixel lands on circle
                            // 
                            // started with <= but moved to < per Figure 5 & Piazza Q@14. 
                            // Difference is drawing 1 extra pixel on circle boundary. See Piazza Q@14 for details.
                            if (distance_from_center < curr_circle.radius) {
                                current_pixel = curr_circle.color * curr_circle.alpha + (1 - curr_circle.alpha) * current_pixel;

                                //alpha_layers.push_back(curr_circle.color);
                                //alphas.push_back(curr_circle.alpha);
                                //num_layers++;
                            }
                        }
                        else if (auto* rectangle = std::get_if<Rectangle>(&shape)) {
                            const Rectangle& curr_rectangle = *rectangle;
                            // do something with rectangle

                            // Vector2:
                            //  curr_rectangle.p_min top-left point
                            //  curr_rectangle.p_max bottom-right point

                            // get object space
                            // translate given coords to object space of shape
                            Vector3 current_object_space = object_space(ssX, ssY, rectangle->transform);

                            bool within_x_bounds = (current_object_space.x > curr_rectangle.p_min.x &&
                                current_object_space.x < curr_rectangle.p_max.x);
                            bool within_y_bounds = (current_object_space.y > curr_rectangle.p_min.y &&
                                current_object_space.y < curr_rectangle.p_max.y);
                            if (within_x_bounds && within_y_bounds) {
                                current_pixel = curr_rectangle.color * curr_rectangle.alpha + (1 - curr_rectangle.alpha) * current_pixel;
                                //alpha_layers.push_back(curr_rectangle.color);
                                //alphas.push_back(curr_rectangle.alpha);
                                //num_layers++;
                            }
                        }
                        else if (auto* triangle = std::get_if<Triangle>(&shape)) {
                            const Triangle& curr_triangle = *triangle;
                            // do something with triangle
                            // p0, p1, p2

                            // get object space
                            // translate given coords to object space of shape
                            Vector3 current_object_space = object_space(ssX, ssY, triangle->transform);

                            // edge vectors
                            Vector2 edge1 = curr_triangle.p1 - curr_triangle.p0;
                            Vector2 edge2 = curr_triangle.p2 - curr_triangle.p1;
                            Vector2 edge3 = curr_triangle.p0 - curr_triangle.p2;

                            // edge normal vectors
                            Vector2 n01 = Vector2{ edge1.y, -edge1.x };
                            Vector2 n12 = Vector2{ edge2.y, -edge2.x };
                            Vector2 n20 = Vector2{ edge3.y, -edge3.x };

                            // current point vectors
                            Vector2 pv0 = Vector2{ curr_triangle.p0.x - current_object_space.x, curr_triangle.p0.y - current_object_space.y };
                            Vector2 pv1 = Vector2{ curr_triangle.p1.x - current_object_space.x, curr_triangle.p1.y - current_object_space.y };
                            Vector2 pv2 = Vector2{ curr_triangle.p2.x - current_object_space.x, curr_triangle.p2.y - current_object_space.y };

                            Real d0 = dot(n01, pv0);
                            Real d1 = dot(n12, pv1);
                            Real d2 = dot(n20, pv2);

                            bool within_triangle = (d0 >= 0 && d1 >= 0 && d2 >= 0) || (d0 <= 0 && d1 <= 0 && d2 <= 0);

                            if (within_triangle) {
                                current_pixel = curr_triangle.color * curr_triangle.alpha + (1 - curr_triangle.alpha) * current_pixel;
                                //alpha_layers.push_back(curr_triangle.color);
                                //alphas.push_back(curr_triangle.alpha);
                                //num_layers++;
                            }
                        }


                    }

                    //std::cout << "num_layers: " << num_layers << ", alpha_layers.size(): " << alpha_layers.size() << "\n";
                    //std::cout << "alphas: " << ", alphas.size(): " << alphas.size() << "\n";

                    // back --> front. -1
                    /*Vector3 background_leftover = Vector3{0,0,0};
                    if (num_layers > 0) {
                        current_pixel = alpha_layers[num_layers-1] * alphas[num_layers-1];
                        //background_leftover *= (1 - alphas[0]);

                        if (num_layers == 2) {
                            current_pixel += alphas[num_layers - 2] * (1 - alphas[num_layers - 1]) * alpha_layers[num_layers - 2];
                        }
                        for (size_t i = num_layers - 2; i < num_layers-2; i--) {
                            background_leftover *= (1 - alphas[i]);

                            //current_pixel += alphas[i] * (1 - alphas[i + 1]) * alpha_layers[i];
                        }
                        //current_pixel += background_leftover * scene.background;
                    } else {
                        current_pixel = scene.background;
                    }*/
                    

                    // save current pixel to the SSAA array
                    // https://cplusplus.com/reference/vector/vector/push_back/
                    sub_pixels.push_back(current_pixel);
                    //std::cout << "hello\n";

                }
            }

            // average the current pixel's (x,y) SSAA array (scale of SSAA_factor).
            // r g b
            Vector3 average_pixel = Vector3{ 0,0,0 };
            //std::cout << size(sub_pixels) << "=======================================================================\n";
            for (size_t k = 0; k < size(sub_pixels); k++) {
                average_pixel.x += sub_pixels[k].x;
                average_pixel.y += sub_pixels[k].y;
                average_pixel.z += sub_pixels[k].z;
            }
            //std::cout << "done\n\n";

            average_pixel.x = average_pixel.x / size(sub_pixels);
            average_pixel.y = average_pixel.y / size(sub_pixels);
            average_pixel.z = average_pixel.z / size(sub_pixels);

            img(x, y) = average_pixel;

        }
    }
    return img;
}
