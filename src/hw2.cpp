#include "hw2.h"
#include "hw2_scenes.h"

using namespace hw2;

Real xcameraToImageSpace(Real x, Real width, Real s, Real a) {
    Real xpp = 0;

    // homework2.pdf formula (2) .. x''
    xpp = width*((x+s*a)/(2*s*a)); // given

    return xpp;
}
Real ycameraToImageSpace(Real y, Real height, Real s, Real a) {
    Real ypp = 0;

    ypp = (height*((-y+s)/(2*s))); // assumption

    return ypp;
}

bool debug_hw2_1 = false;
bool debug = false;
void bDebug(std::string section, std::string message, bool newline = true) {
    if (debug) {
        std::string debug_section = "hw2_" + section;
        if (newline) {
            std::cout << "[" << debug_section << section << "] " << message << "\n";
        } else {
            std::cout << message;
        }
    }
}

bool pointInTriangle(Vector2 p0, Vector2 p1, Vector2 p2, Real x, Real y) {
    // We can determine whether a point q is on the positive or negative half-planes 
    // by looking at the sign of the dot product between a vector formed by the point q 
    // and the line, and a normal vector n that is perpendicular to the direction of the line.
    //

    // sign of the dot product between a vector formed by the point q and the line
    // a normal vector n that is perpendicular to the direction of the line


    // edge vectors
    Vector2 edge1 = p1 - p0;
    Vector2 edge2 = p2 - p1;
    Vector2 edge3 = p0 - p2;

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
    Vector2 pv0 = Vector2{ p0.x - x, p0.y - y };
    Vector2 pv1 = Vector2{ p1.x - x, p1.y - y };
    Vector2 pv2 = Vector2{ p2.x - x, p2.y - y };

    Real d0 = dot(n01, pv0);
    Real d1 = dot(n12, pv1);
    Real d2 = dot(n20, pv2);

    // boolean statement, true if in triangle
    return ((d0 >= 0 && d1 >= 0 && d2 >= 0) || (d0 <= 0 && d1 <= 0 && d2 <= 0));
}

Vector3 averagePixel(std::vector<Vector3> sub_pixels) {
    // average the current pixel's (x,y) SSAA array (scale of SSAA_factor).
    // r g b
    Vector3 average_pixel = Vector3{0,0,0};
    //std::cout << size(sub_pixels) << "=======================================================================\n";
    for (size_t k = 0; k < size(sub_pixels); k++) {
        average_pixel.x += sub_pixels[k].x;
        average_pixel.y += sub_pixels[k].y;
        average_pixel.z += sub_pixels[k].z;
    }

    average_pixel.x = average_pixel.x / size(sub_pixels);
    average_pixel.y = average_pixel.y / size(sub_pixels);
    average_pixel.z = average_pixel.z / size(sub_pixels);

    return average_pixel;
}

Image3 hw_2_1(const std::vector<std::string> &params) {
    // Homework 2.1: render a single 3D triangle

    Image3 img(640 /* width */, 480 /* height */);

    Real a = Real(img.width) / Real(img.height); // aspect ratio

    Vector3 p0{0, 0, -1};
    Vector3 p1{1, 0, -1};
    Vector3 p2{0, 1, -1};
    Real s = 1; // scaling factor of the view frustrum
    Vector3 color = Vector3{1.0, 0.5, 0.5};
    Real z_near = 1e-6; // distance of the near clipping plane
    // get passed parameters
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-p0") {
            p0.x = std::stof(params[++i]);
            p0.y = std::stof(params[++i]);
            p0.z = std::stof(params[++i]);
        } else if (params[i] == "-p1") {
            p1.x = std::stof(params[++i]);
            p1.y = std::stof(params[++i]);
            p1.z = std::stof(params[++i]);
        } else if (params[i] == "-p2") {
            p2.x = std::stof(params[++i]);
            p2.y = std::stof(params[++i]);
            p2.z = std::stof(params[++i]);
        } else if (params[i] == "-color") {
            Real r = std::stof(params[++i]);
            Real g = std::stof(params[++i]);
            Real b = std::stof(params[++i]);
            color = Vector3{r, g, b};
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        }
    }

    if (debug_hw2_1) {
    std::cout << "p0: " << p0 << "\n";
    std::cout << "p1: " << p1 << "\n";
    std::cout << "p2: " << p2 << "\n";
    }

    // define projected points
    Vector2 pp0{0,0};
    pp0.x = -p0.x/p0.z;
    pp0.y = -p0.y/p0.z;

    Vector2 pp1{0,0};
    pp1.x = -p1.x/p1.z;
    pp1.y = -p1.y/p1.z;
    
    Vector2 pp2{0,0};
    pp2.x = -p2.x/p2.z;
    pp2.y = -p2.y/p2.z;

    if (debug_hw2_1) {
    std::cout << "pp0: " << pp0 << "\n";
    std::cout << "pp1: " << pp1 << "\n";
    std::cout << "pp2: " << pp2 << "\n";
    }

    // convert projected points (in camera space) to image space
    pp0.x = xcameraToImageSpace(pp0.x, img.width, s, a);
    pp0.y = ycameraToImageSpace(pp0.y, img.height, s, a);

    pp1.x = xcameraToImageSpace(pp1.x, img.width, s, a);
    pp1.y = ycameraToImageSpace(pp1.y, img.height, s, a);

    pp2.x = xcameraToImageSpace(pp2.x, img.width, s, a);
    pp2.y = ycameraToImageSpace(pp2.y, img.height, s, a);
    
    if (debug_hw2_1) {
    std::cout << "pp0: " << pp0 << "\n";
    std::cout << "pp1: " << pp1 << "\n";
    std::cout << "pp2: " << pp2 << "\n";
    }

    // image plane is at z = -1
    // projected point(s) (x',y') = (-x/z,-y/z)
    // extent of the image plane = [-sa,-s,-1]x[sa,s,-1]
    
    // s = scale factor = tan(a/2)
    // a = aspect ratio (image width/image height)

    // BEGIN IMPORT FROM HW1

    Real SSAA_factor = 4;
    // for each pixel
    //
    // circles are ordered from the farthest to the closest.
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            // y
            // x
            // color

            std::vector<Vector3> sub_pixels;
            Vector3 current_pixel;

            for (Real ssY = y; ssY < y + 1; ssY += (1.0 / SSAA_factor)) {
                for (Real ssX = x; ssX < x + 1; ssX += (1.0 / SSAA_factor)) {

                    //std::cout << "x: " << x << " y: " << y << " ssX: " << ssX << " ssY: " << ssY << "\n";
                    // background color
                    current_pixel = Vector3{ 0.5,0.5,0.5 };

                    // if pixel center hits triangle
                    if (pointInTriangle(pp0, pp1, pp2, ssX, ssY)) {
                        current_pixel = color;

                        bDebug("1", "in triangle!");  //if (debug_hw2_1) { std::cout << "in triangle!\n"; }
                    }
                    
                    // save current pixel to the SSAA array
                    // https://cplusplus.com/reference/vector/vector/push_back/
                    sub_pixels.push_back(current_pixel);

                }
            }

            // average the current pixel's (x,y) SSAA array (scale of SSAA_factor).
            img(x, y) = averagePixel(sub_pixels);

        }
    }
    return img;
}

Image3 hw_2_2(const std::vector<std::string> &params) {
    // Homework 2.2: render a triangle mesh

    Image3 img(640 /* width */, 480 /* height */);

    Real a = Real(img.width) / Real(img.height); // aspect ratio

    Real s = 1; // scaling factor of the view frustrum
    Real z_near = 1e-6; // distance of the near clipping plane
    int scene_id = 0;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        } else if (params[i] == "-scene_id") {
            scene_id = std::stoi(params[++i]);
        }
    }

    TriangleMesh mesh = meshes[scene_id];
    //UNUSED(mesh); // silence warning, feel free to remove this

    Image3 z_buffer = Image3(img.width, img.height);
    // full of x,y,z. z 0is the depth of the pixel. stores the closest Z value for each pixel. updated by the below loop.
    
    std::cout << "Size of mesh.vertices: " << mesh.vertices.size() << "\n";
    std::cout << "Size of mesh.faces: " << mesh.faces.size() << "\n";

    /*
    i: 0
    mesh.faces[i]: (0, 1, 2)
    mesh.vertices[mesh.faces[i].x]: (-1.7, 1, -5)
    mesh.vertices[mesh.faces[i].y]: (1, 1, -5)
    mesh.vertices[mesh.faces[i].z]: (-0.5, -1, -5)
    mesh.face_colors[i]: (0.35, 0.75, 0.35)
    */

    // for each triangle in the mesh 
    for (size_t i = 0; i < mesh.faces.size(); ++i) {
        std::cout << "i: " << i << "\n";
        std::cout << "mesh.faces[i]: " << mesh.faces[i] << "\n";
        std::cout << "mesh.vertices[mesh.faces[i].x]: " << mesh.vertices[mesh.faces[i].x] << "\n";
        std::cout << "mesh.vertices[mesh.faces[i].y]: " << mesh.vertices[mesh.faces[i].y] << "\n";
        std::cout << "mesh.vertices[mesh.faces[i].z]: " << mesh.vertices[mesh.faces[i].z] << "\n";
        std::cout << "mesh.face_colors[i]: " << mesh.face_colors[i] << "\n";
        

        // get the 3 vertices of the triangle
        Vector3 p0 = mesh.vertices[mesh.faces[i].x];
        Vector3 p1 = mesh.vertices[mesh.faces[i].y];
        Vector3 p2 = mesh.vertices[mesh.faces[i].z];

        // define projected points (onto image plane)
        Vector2 pp0{0,0};
        pp0.x = -p0.x/p0.z;
        pp0.y = -p0.y/p0.z;

        Vector2 pp1{0,0};
        pp1.x = -p1.x/p1.z;
        pp1.y = -p1.y/p1.z;
        
        Vector2 pp2{0,0};
        pp2.x = -p2.x/p2.z;
        pp2.y = -p2.y/p2.z;

        // convert projected points (in camera space) to image space
        pp0.x = xcameraToImageSpace(pp0.x, img.width, s, a);
        pp0.y = ycameraToImageSpace(pp0.y, img.height, s, a);

        pp1.x = xcameraToImageSpace(pp1.x, img.width, s, a);
        pp1.y = ycameraToImageSpace(pp1.y, img.height, s, a);

        pp2.x = xcameraToImageSpace(pp2.x, img.width, s, a);
        pp2.y = ycameraToImageSpace(pp2.y, img.height, s, a);

        // now loop for each pixel in the image
        // TODO: ONLY do this for pixels within the triangle bounding box to simplify complexity. This would be in image space.
        for (int y = 0; y < img.height; y++) {
            for (int x = 0; x < img.width; x++) {
                // if pixel center hits triangle
                //img(x, y) = Vector3{1, 1, 1};
                if (pointInTriangle(pp0, pp1, pp2, x, y)) {
                    // get the depth of the pixel
                    Real z = -1.0; // TODO: get the depth of the pixel
                    // if the depth of the pixel is less than the current depth in the z_buffer, update the z_buffer and the pixel color
                    if (z < z_buffer(x, y).z) {
                        // update the z_buffer
                        z_buffer(x, y).z = z;
                        // update the pixel color
                        img(x, y) = mesh.face_colors[i];
                    }
                }
            }
        }

    }


    return img;
}

Image3 hw_2_3(const std::vector<std::string> &params) {
    // Homework 2.3: render a triangle mesh with vertex colors

    Image3 img(640 /* width */, 480 /* height */);

    Real s = 1; // scaling factor of the view frustrum
    Real z_near = 1e-6; // distance of the near clipping plane
    int scene_id = 0;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        } else if (params[i] == "-scene_id") {
            scene_id = std::stoi(params[++i]);
        }
    }

    TriangleMesh mesh = meshes[scene_id];
    UNUSED(mesh); // silence warning, feel free to remove this

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{1, 1, 1};
        }
    }
    return img;
}

Image3 hw_2_4(const std::vector<std::string> &params) {
    // Homework 2.4: render a scene with transformation
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.camera.resolution.x,
               scene.camera.resolution.y);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{1, 1, 1};
        }
    }
    return img;
}

