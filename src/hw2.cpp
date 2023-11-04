#include "hw2.h"
#include "hw2_scenes.h"

using namespace hw2;

Real xcameraToImageSpace(Real x, Real width, Real s, Real a) {
    Real xpp = 0;

    // homework2.pdf formula (2) .. x''
    xpp = width*((x+s*a)/(2*s*a)); // given

    return xpp;
}
Real ycameraToImageSpace(Real y, Real height, Real s) {
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
    pp0.y = ycameraToImageSpace(pp0.y, img.height, s);

    pp1.x = xcameraToImageSpace(pp1.x, img.width, s, a);
    pp1.y = ycameraToImageSpace(pp1.y, img.height, s);

    pp2.x = xcameraToImageSpace(pp2.x, img.width, s, a);
    pp2.y = ycameraToImageSpace(pp2.y, img.height, s);
    
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

Real parallelogramArea3D(Vector3 A, Vector3 B) {
    // cross product of the two edge vectors
    Vector3 X = cross(A, B);

    // https://math.stackexchange.com/q/643227
    // length of the cross product
    Real x_len = length(X);

    return x_len;
}
// returns the area of a given 3D triangle based on its three point vertices
Real triangleArea3D(Vector3 A, Vector3 B, Vector3 C) {
    // area of triangle = area of parallelogram /2 = ||u x v|| / 2
    // https://mathsathome.com/vector-between-2-points/ 
    // from the three point vectors, get the two edge vectors for the cross product.
    // co-pilot: then get the area of the parallelogram formed by the two edge vectors.
    // co-pilot: then divide by 2 to get the area of the triangle.
    // area(A,B,C) = 1/2 * ||(B-A) x (C-A)|| .... which is basically (A->B), (A->C) 
    // https://math.stackexchange.com/questions/128991/how-to-calculate-area-of-3d-triangle

    // A->B = (B-A)
    Vector3 AtoB = B - A;
    // A->C = (C-A)
    Vector3 AtoC = C - A; // this can be B->C or (C-B), and will still work same (tested, but not every scene, or sure why)

    Real parallelogram_area = parallelogramArea3D(AtoB, AtoC); // this had / Real(2) (which is redundant with the return statement) and still worked??? wtf

    // divide by 2 to get the area of the triangle.
    return parallelogram_area / Real(2);
}
// returns the area of a given 2D triangle based on its three point vertices
Real triangleArea2D(Vector2 A, Vector2 B, Vector2 C) {
    // Substantiate the 3D points with z=0
    return triangleArea3D(Vector3{A.x, A.y, 0.0}, Vector3{B.x, B.y, 0.0}, Vector3{C.x, C.y, 0.0});
}

struct barycentric_coordinates {
    Real b0;
    Real b1;
    Real b2;
};

barycentric_coordinates convertToBarycentric(Vector3 p0, Vector3 p1, Vector3 p2,  
                                             Vector2 pp0, Vector2 pp1, Vector2 pp2,  
                                             Real x, Real y) {
    // vertices of 3D-native triangle: <p0>, <p1>, <p2>
    // vertices of projected triangle: (pp0), (pp1), (pp2)
    // current point (pixel): (x, y) -- 'projected' as on image-plane

    barycentric_coordinates barycentric;

    // 1. Compute projected (image space) barycentric coordinates. b'0 b'1 b'2 (Eq.5)
    // 2. Find original barycentric coordinates using the projected ones and depth at the three vertices. (Eq.14)

    // projected barycentric coordinates
    //
    Real projected_barycentric_denominator = triangleArea2D(pp0, pp1, pp2); // area(pp0, pp1, pp2)
    // b'0 = area(p, pp1, pp2) / area(pp0, pp1, pp2)
    Real bp0 = triangleArea2D(Vector2{x,y}, pp1, pp2) / projected_barycentric_denominator;
    // b'1 = area(pp0, p, pp2) / area(pp0, pp1, pp2)
    Real bp1 = triangleArea2D(pp0, Vector2{x,y}, pp2) / projected_barycentric_denominator;
    // b'2 = area(pp0, pp1, p) / area(pp0, pp1, pp2)
    Real bp2 = triangleArea2D(pp0, pp1, Vector2{x,y}) / projected_barycentric_denominator;

    // original barycentric coordinates
    //
    Real original_barycentric_denominator = ((bp0/p0.z)+(bp1/p1.z)+(bp2/p2.z));
    // b0
    Real b0 = (bp0/p0.z)/original_barycentric_denominator;
    // b1
    Real b1 = (bp1/p1.z)/original_barycentric_denominator;
    // b2
    Real b2 = (bp2/p2.z)/original_barycentric_denominator;

    //barycentric.b0 = b0, barycentric.b1 = b1, barycentric.b2 = b2;
    // simplify the above line
    barycentric = {b0, b1, b2};

    return barycentric;
}

Real depthOfPixel(Vector3 p0, Vector3 p1, Vector3 p2,  
                  Vector2 pp0, Vector2 pp1, Vector2 pp2,  
                  Real x, Real y) {
    // get the depth of the pixel

    // z = b0*p0.z + b1*p1.z + b2*p2.z

    // 1. Compute projected (image space) barycentric coordinates. b'0 b'1 b'2 (Eq.5)
    // 2. Find original barycentric coordinates using the projected ones and depth at the three vertices. (Eq.14)
    // 3. Finally, obtain depth using Eq.15

    barycentric_coordinates barycentric = convertToBarycentric(p0, p1, p2, pp0, pp1, pp2, x, y);

    // final depth formula, of the pixel
    Real z = barycentric.b0 * p0.z + barycentric.b1 * p1.z + barycentric.b2 * p2.z;

    return z;
}
// overloaded for HW2_3 & beyond
Real depthOfPixel(Vector3 p0, Vector3 p1, Vector3 p2, barycentric_coordinates barycentric) {
    // final depth formula, of the pixel
    Real z = barycentric.b0 * p0.z + barycentric.b1 * p1.z + barycentric.b2 * p2.z;

    return z;
}

Image3 hw_2_2(const std::vector<std::string> &params) {
    // Homework 2.2: render a triangle mesh

    Image3 return_img(640 /* width */, 480 /* height */);
    Real SSAA_factor = 4;
    Image3 img(return_img.width*SSAA_factor /* width */, return_img.height*SSAA_factor /* height */);

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

    Real furthest_z_depth = -1000000; // infinity. 
    Image3 z_buffer = Image3(img.width, img.height);
    //std::cout << "init" << "\n";
    //std::cout << "img.height: " << img.height << "\n";
    //std::cout << "img.width: " << img.width << "\n";
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            z_buffer(x, y) = Vector3{0.0, 0.0, furthest_z_depth}; // max z_buffer depth.
            img(x, y) = Vector3{0.5, 0.5, 0.5}; // background color of image


            //std::cout << "within" << "\n";
            //std::cout << "img(" << x << "," << y << "): " << img(x, y) << "\n";
        }
    }
    //std::cout << "end init" << "\n";
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
        pp0.y = ycameraToImageSpace(pp0.y, img.height, s);

        pp1.x = xcameraToImageSpace(pp1.x, img.width, s, a);
        pp1.y = ycameraToImageSpace(pp1.y, img.height, s);

        pp2.x = xcameraToImageSpace(pp2.x, img.width, s, a);
        pp2.y = ycameraToImageSpace(pp2.y, img.height, s);

        // now loop for each pixel in the image
        // TODO: ONLY do this for pixels within the triangle bounding box to simplify complexity. This would be in image space.
        for (int y = 0; y < img.height; y++) {
            for (int x = 0; x < img.width; x++) {
                // if pixel center hits triangle
                //img(x, y) = Vector3{1, 1, 1};

                //std::cout << "NIT"; // not in triangle
                if (pointInTriangle(pp0, pp1, pp2, x, y)) {
                    

                    Real z_depth = depthOfPixel(p0, p1, p2, pp0, pp1, pp2, x, y);

                    //std::cout << "\n\n[" << i << "]" << " pixel: " << Vector2{x,y} << " current z_depth: " << z_depth << " | stored z_buffer: " << z_buffer(x,y).z; 

                    // if the depth of the pixel is less than the current depth in the z_buffer, update the z_buffer and the pixel color
                    if (z_depth > z_buffer(x, y).z) {

                        //std::cout << "********** OVERRIDING!!! **********";

                        // update the z_buffer
                        z_buffer(x, y).z = z_depth;
                        // update the pixel color
                        img(x, y) = mesh.face_colors[i];
                    }

                    //std::cout << "\n";
                }
            }
        }
    }

    std::cout << "merging\n";

    // Scale SSAA image down to return_image size (Apply SSAA)
    // for each pixel in the return image (non-SSAA scaled, smaller image)
    for (int y = 0; y < return_img.height; y++) {
        for (int x = 0; x < return_img.width; x++) {
            //std::cout << "x: " << x << " y: " << y << "\n";
            Vector3 avg_color = Vector3{0,0,0};

            // for each sub-pixel (in the larger SSAA scaled, larger image) at the current pixel
            for (int dx = 0; dx < SSAA_factor; dx++) {
                for (int dy = 0; dy < SSAA_factor; dy++) {
                    //std::cout << "dx: " << dx << " dy: " << dy << "\n";
                    // (SSAA_factor) * (current x position) + (current sub x position)
                    Real curr_x = SSAA_factor * x + dx;
                    Real curr_y = SSAA_factor * y + dy;
                    //std::cout << "curr_x: " << curr_x << " curr_y: " << curr_y << "\n";
                    Vector3 current_color = img(curr_x, curr_y);
                    //std::cout << "current_color: " << current_color << "\n";

                    avg_color += current_color;

                }
            }
            // populate the return image with the average color from the larger image
            //std::cout << "avg_color: " << avg_color << "\n";
            return_img(x, y) = avg_color / Real(SSAA_factor * SSAA_factor);
        }
    }

    return return_img;
}

Vector3 colorOfPixel(Vector3 p0, Vector3 p1, Vector3 p2, barycentric_coordinates barycentric) {
    Vector3 color = barycentric.b0 * p0 + barycentric.b1 * p1 + barycentric.b2 * p2;

    return color;
}

Image3 hw_2_3(const std::vector<std::string> &params) {
    // Homework 2.3: render a triangle mesh with vertex colors

    Image3 return_img(640 /* width */, 480 /* height */);
    Real SSAA_factor = 4;
    Image3 img(return_img.width*SSAA_factor /* width */, return_img.height*SSAA_factor /* height */);

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
    std::cout << "Size of mesh.vertices: " << mesh.vertices.size() << "\n";
    std::cout << "Size of mesh.faces: " << mesh.faces.size() << "\n";

    Real furthest_z_depth = -1000000; // infinity. 
    // full of x,y,z. z 0is the depth of the pixel. stores the closest Z value for each pixel. updated by the below loop.
    Image3 z_buffer = Image3(img.width, img.height);
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            z_buffer(x, y) = Vector3{0.0, 0.0, furthest_z_depth}; // max z_buffer depth.
            img(x, y) = Vector3{0.5, 0.5, 0.5}; // background color of image
        }
    }

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
        pp0.y = ycameraToImageSpace(pp0.y, img.height, s);

        pp1.x = xcameraToImageSpace(pp1.x, img.width, s, a);
        pp1.y = ycameraToImageSpace(pp1.y, img.height, s);

        pp2.x = xcameraToImageSpace(pp2.x, img.width, s, a);
        pp2.y = ycameraToImageSpace(pp2.y, img.height, s);

        // now loop for each pixel in the image
        // TODO: ONLY do this for pixels within the triangle bounding box to simplify complexity. This would be in image space.
        for (int y = 0; y < img.height; y++) {
            for (int x = 0; x < img.width; x++) {
                // if pixel center hits triangle
                if (pointInTriangle(pp0, pp1, pp2, x, y)) {
                    
                    barycentric_coordinates barycentric = convertToBarycentric(p0, p1, p2, pp0, pp1, pp2, x, y);

                    Real z_depth = depthOfPixel(p0, p1, p2, barycentric);

                    // if the depth of the pixel is less than the current depth in the z_buffer, update the z_buffer and the pixel color
                    if (z_depth > z_buffer(x, y).z) {

                        Vector3 c0 = mesh.vertex_colors[mesh.faces[i].x];
                        Vector3 c1 = mesh.vertex_colors[mesh.faces[i].y];
                        Vector3 c2 = mesh.vertex_colors[mesh.faces[i].z];

                        Vector3 color = colorOfPixel(c0, c1, c2, barycentric);

                        // update the z_buffer
                        z_buffer(x, y).z = z_depth;
                        // update the pixel color
                        img(x, y) = color;
                    }
                }
            }
        }
    }

    // Scale SSAA image down to return_image size (Apply SSAA)
    // for each pixel in the return image (non-SSAA scaled, smaller image)
    for (int y = 0; y < return_img.height; y++) {
        for (int x = 0; x < return_img.width; x++) {
            Vector3 avg_color = Vector3{0,0,0};

            // for each sub-pixel (in the larger SSAA scaled, larger image) at the current pixel
            for (int dx = 0; dx < SSAA_factor; dx++) {
                for (int dy = 0; dy < SSAA_factor; dy++) {
                    // (SSAA_factor) * (current x position) + (current sub x position)
                    Real curr_x = SSAA_factor * x + dx;
                    Real curr_y = SSAA_factor * y + dy;

                    Vector3 current_color = img(curr_x, curr_y);

                    avg_color += current_color;

                }
            }
            // populate the return image with the average color from the larger image
            return_img(x, y) = avg_color / Real(SSAA_factor * SSAA_factor);
        }
    }

    return return_img;
}

Image3 hw_2_4(const std::vector<std::string> &params) {
    // Homework 2.4: render a scene with transformation
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    std::cout << "params[0]: " << params[0] << "\n";
    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 return_img(scene.camera.resolution.x,
               scene.camera.resolution.y);

    Real SSAA_factor = 4;
    Image3 img(return_img.width*SSAA_factor /* width */, return_img.height*SSAA_factor /* height */);

    // Object Space
    //    v  model matrix
    // World Space
    //    v  view matrix
    // Camera Space
    //    v  projection matrix
    // Screen Space
    // 
    // given vertex v on 3D triangle mesh, we can project to screen space using:
    // v' = PVMv = P * V * M * v

    Real a = Real(img.width) / Real(img.height); // aspect ratio
    Real s = scene.camera.s; // scaling factor of the view frustrum
    Real z_near = scene.camera.z_near; // distance of the near clipping plane

    Real furthest_z_depth = -1000000; // infinity. 
    // full of x,y,z. z 0is the depth of the pixel. stores the closest Z value for each pixel. updated by the below loop.
    Image3 z_buffer = Image3(img.width, img.height);
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            z_buffer(x, y) = Vector3{0.0, 0.0, furthest_z_depth}; // max z_buffer depth.
            img(x, y) = scene.background; // background color of image
        }
    }


    // world to camera
    Matrix4x4 V = inverse(scene.camera.cam_to_world);

    std::cout << V << "\n";

    // perspective projection
    // takes point from object space to screen space.

    // another matrix is needed for to screen space.
    // this is world to camera space. (could be object if no transformations are applied)
    // view matrix V (Fig.12)
    Matrix4x4 view_matrix = Matrix4x4::identity(); // P1
    view_matrix(2,2) = 0;
    view_matrix(2,3) = 1;
    view_matrix(3,2) = -1;
    view_matrix(3,3) = 0;
    // * x,y,z,1 = x',y',z',w' (1 column)

    std::cout << view_matrix << "\n";

    //view_matrix should be = to V

    // camera space to screen space
    // projection matrix
    Matrix4x4 projection_matrix = Matrix4x4::identity(); // P2
    projection_matrix(0,0) = img.width/(2*s*a);
    projection_matrix(1,1) = -img.height/(2*s);
    projection_matrix(0,3) = img.width/Real(2);
    projection_matrix(1,3) = img.height/Real(2);
    // TODO: understand this better ^^^

    std::cout << projection_matrix << "\n";

    // for each mesh in the scene
    for (TriangleMesh mesh : scene.meshes) {
        std::cout << "Size of mesh.vertices: " << mesh.vertices.size() << "\n";
        std::cout << "Size of mesh.faces: " << mesh.faces.size() << "\n";
        // for each triangle in the mesh 
        for (size_t i = 0; i < mesh.faces.size(); ++i) {
            std::cout << "i: " << i << "\n";
            std::cout << "mesh.faces[i]: " << mesh.faces[i] << "\n";
            std::cout << "mesh.vertices[mesh.faces[i].x]: " << mesh.vertices[mesh.faces[i].x] << "\n";
            std::cout << "mesh.vertices[mesh.faces[i].y]: " << mesh.vertices[mesh.faces[i].y] << "\n";
            std::cout << "mesh.vertices[mesh.faces[i].z]: " << mesh.vertices[mesh.faces[i].z] << "\n";
            //std::cout << "mesh.face_colors[i]: " << mesh.face_colors[i] << "\n";
            

            // get the 3 vertices of the triangle
            Vector3 p0 = mesh.vertices[mesh.faces[i].x];
            Vector3 p1 = mesh.vertices[mesh.faces[i].y];
            Vector3 p2 = mesh.vertices[mesh.faces[i].z];

            // object to world
            // logic in hw2_scenes.cpp
            Matrix4x4 M = mesh.model_matrix; 

            // v' = PVMv 
            /*
            Vector4 tp0 = projection_matrix * V * model_matrix * Vector4{p0.x, p0.y, p0.z, 1.0};
            Vector4 tp1 = projection_matrix * V * model_matrix * Vector4{p1.x, p1.y, p1.z, 1.0};
            Vector4 tp2 = projection_matrix * V * model_matrix * Vector4{p2.x, p2.y, p2.z, 1.0};*/

            /*std::cout << "p0: " << p0 << "\n";
            std::cout << "p1: " << p1 << "\n";
            std::cout << "p2: " << p2 << "\n";*/

            Vector4 tt0 = V * M * Vector4{p0.x, p0.y, p0.z, Real(1.0)};
            Vector4 tt1 = V * M * Vector4{p1.x, p1.y, p1.z, Real(1.0)};
            Vector4 tt2 = V * M * Vector4{p2.x, p2.y, p2.z, Real(1.0)};

            /*std::cout << "tt0: " << tt0 << "\n";
            std::cout << "tt1: " << tt1 << "\n";
            std::cout << "tt2: " << tt2 << "\n";*/

            p0 = Vector3{tt0.x, tt0.y, tt0.z};
            p1 = Vector3{tt1.x, tt1.y, tt1.z};
            p2 = Vector3{tt2.x, tt2.y, tt2.z};
            
            /*
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
            pp0.y = ycameraToImageSpace(pp0.y, img.height, s);

            pp1.x = xcameraToImageSpace(pp1.x, img.width, s, a);
            pp1.y = ycameraToImageSpace(pp1.y, img.height, s);

            pp2.x = xcameraToImageSpace(pp2.x, img.width, s, a);
            pp2.y = ycameraToImageSpace(pp2.y, img.height, s);*/

            Vector4 wp0 = projection_matrix * view_matrix * tt0;
            Vector4 wp1 = projection_matrix * view_matrix * tt1;
            Vector4 wp2 = projection_matrix * view_matrix * tt2;

            Vector2 pp0 = Vector2{wp0.x/wp0.w, wp0.y/wp0.w};
            Vector2 pp1 = Vector2{wp1.x/wp1.w, wp1.y/wp1.w};
            Vector2 pp2 = Vector2{wp2.x/wp2.w, wp2.y/wp2.w};

            /*std::cout << "pp0: " << pp0 << "\n";
            std::cout << "pp1: " << pp1 << "\n";
            std::cout << "pp2: " << pp2 << "\n";*/

            // now loop for each pixel in the image
            // TODO: ONLY do this for pixels within the triangle bounding box to simplify complexity. This would be in image space.
            for (int y = 0; y < img.height; y++) {
                for (int x = 0; x < img.width; x++) {
                    // if pixel center hits triangle
                    if (pointInTriangle(pp0, pp1, pp2, x, y)) {
                        
                        barycentric_coordinates barycentric = convertToBarycentric(p0, p1, p2, pp0, pp1, pp2, x, y);

                        Real z_depth = depthOfPixel(p0, p1, p2, barycentric);

                        // if the depth of the pixel is less than the current depth in the z_buffer, update the z_buffer and the pixel color
                        if (z_depth > z_buffer(x, y).z) {

                            Vector3 c0 = mesh.vertex_colors[mesh.faces[i].x];
                            Vector3 c1 = mesh.vertex_colors[mesh.faces[i].y];
                            Vector3 c2 = mesh.vertex_colors[mesh.faces[i].z];

                            Vector3 color = colorOfPixel(c0, c1, c2, barycentric);

                            // update the z_buffer
                            z_buffer(x, y).z = z_depth;
                            // update the pixel color
                            img(x, y) = color;
                        }
                    }
                }
            }
        }

    }
    // Scale SSAA image down to return_image size (Apply SSAA)
    // for each pixel in the return image (non-SSAA scaled, smaller image)
    for (int y = 0; y < return_img.height; y++) {
        for (int x = 0; x < return_img.width; x++) {
            Vector3 avg_color = Vector3{0,0,0};

            // for each sub-pixel (in the larger SSAA scaled, larger image) at the current pixel
            for (int dx = 0; dx < SSAA_factor; dx++) {
                for (int dy = 0; dy < SSAA_factor; dy++) {
                    // (SSAA_factor) * (current x position) + (current sub x position)
                    Real curr_x = SSAA_factor * x + dx;
                    Real curr_y = SSAA_factor * y + dy;

                    Vector3 current_color = img(curr_x, curr_y);

                    avg_color += current_color;

                }
            }
            // populate the return image with the average color from the larger image
            return_img(x, y) = avg_color / Real(SSAA_factor * SSAA_factor);
        }
    }

    return return_img;
}

