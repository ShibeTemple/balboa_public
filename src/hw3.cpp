#include "hw3.h"
#include "3rdparty/glad.h" // GLAD. OpenGL loader. needs to be included before GLFW!
#include "3rdparty/glfw/include/GLFW/glfw3.h" // GLFW. Window and input handler.
#include "hw3_scenes.h"
#include "3rdparty/stb/stb_image.h" // texture handling

using namespace hw3;

// GLSL shader source for HW3_2
// rotation too
const char* vertexShaderSource = "#version 330 core\n" 
    "layout (location = 0) in vec3 aPos;\n"
    "uniform float rotationAngle;\n" // single precision warning. (GLSL:1,balboa:2)
    "void main()\n"
    "{\n"
    "   mat2 rotationMatrix = mat2(cos(rotationAngle), -sin(rotationAngle),\n"
    "                           sin(rotationAngle), cos(rotationAngle));\n"
    "   vec2 rotatedPosition = rotationMatrix * aPos.xy;\n"
    //"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "   gl_Position = vec4(rotatedPosition, aPos.z, 1.0);\n"
    "}\0";
const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";


// Camera and timing attributes
Vector3f cameraPos, cameraFront, cameraUp;
float z_far, z_near;
Vector3f lightPos = Vector3f{ 1.0, 1.0, 1.0 }; // lighting

// timing global attributes
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


// callback functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
void processInput(GLFWwindow* window) {
    // returns whether this key is currently being pressed.
    // no = GLFW_RELEASE, yes = GLFW_PRESS
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true); // breaks main render loop, due to condition.

    float cameraSpeed = 0.1f * (z_far - z_near) * deltaTime;
    // w
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    // a
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUp;
    // s
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    // d
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp;
}


// Function to compile and check shader
unsigned int compileShader(unsigned int type, const char* source) {
    unsigned int id = glCreateShader(type);
    glShaderSource(id, 1, &source, NULL);
    glCompileShader(id);

    // Check for success
    int  success;
    char infoLog[512];
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 0;
    }
    
    return id;
}

// Function to create shader program
unsigned int createShaderProgram(unsigned int  vertexShader, unsigned int fragmentShader) {
    // Shader program: combine all shaders
    unsigned int program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);    
    // check for linking errors
    int  success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glUseProgram(program); // could be exported and not used internally to diversity this functions' applications.

    // cleanup individual shader objects as they are now in shader program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program; // return ID.
}

void hw_3_1(const std::vector<std::string> &params) {
    // HW 3.1: Open a window using GLFW
    
    // GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //mac os 
    #endif

    // Create Application Window
    GLFWwindow* window = glfwCreateWindow(800, 600, "CSE 167 HW3", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);

    // GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    // Create Application Window Viewport Size
    glViewport(0, 0, 800, 600);

    // Define Application Window Resize Callback (gets run on launch as well)
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // render loop
    // An iteration of the render loop is more commonly called a frame
    while (!glfwWindowShouldClose(window)) {
        processInput(window); // user input

        // rendering commands here

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // state-setting function
        glClear(GL_COLOR_BUFFER_BIT); // state-using function

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // cleanup
    glfwTerminate();
    return;
}

void hw_3_2(const std::vector<std::string> &params) {
    // HW 3.2: Render a single 2D triangle

    // GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);    
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //mac os 
    #endif

    // Create Application Window
    GLFWwindow* window = glfwCreateWindow(800, 600, "CSE 167 HW3", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);

    // GLAD: OpenGL function pointers (OS specific)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    // Create Application Window Viewport Size
    glViewport(0, 0, 800, 600);

    // Define Application Window Resize Callback (gets run on launch as well)
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);




    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    unsigned int VBO; // vertex buffer object
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //GL_STREAM_DRAW : the data is set only once and used by the GPU at most a few times.
    //GL_STATIC_DRAW : the data is set only once and used many times.
    //GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);



    // SHADERS:
    // --------
    // define vertex shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // attach source code to object and compile
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // object, # src lines, src.
    glCompileShader(vertexShader);
    // check for success
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // define fragment shader 
    // one output,vector of size 4, defines the final color output 
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Shader program: combine all shaders
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glUseProgram(shaderProgram);
    // cleanup individual shader objects as they are now in shader program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);


    // Get the uniform location
    GLint rotationAngleLocation = glGetUniformLocation(shaderProgram, "rotationAngle");


    // render loop
    // -----------
    // An iteration of the render loop is more commonly called a frame
    while (!glfwWindowShouldClose(window)) {
        processInput(window); // user input

        // rendering commands here
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // state-setting function
        glClear(GL_COLOR_BUFFER_BIT); // state-using function

        // Update rotation angle (adjust the factor for the speed of rotation)
        float rotationAngle = static_cast<float>(glfwGetTime()) * 0.5f;
        

        // 0 draw our first triangle
        glUseProgram(shaderProgram);
        // Set the rotation angle uniform
        glUniform1f(rotationAngleLocation, rotationAngle);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // glBindVertexArray(0); // no need to unbind it every time 

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // cleanup
    glfwTerminate();
    return;
}

void hw_3_3(const std::vector<std::string> &params) {
    // HW 3.3: Render a scene
    if (params.size() == 0) {
        return;
    }
    // handle two 2D triangles first, \
    // add projection matrix, 
    // add view matrix, 
    // add model matrix, 
    // handle multiple triangle meshes, 
    // and finally add camera interaction.

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    // scene attributes
    // ----------------
    int width = scene.camera.resolution.x;
    int height = scene.camera.resolution.y;
    float a = float(width) / height; // aspect ratio
    float s = scene.camera.s; // scaling factor of the view frustrum
    z_near = scene.camera.z_near; // distance of the near clipping plane
    z_far = scene.camera.z_far; // distance of the far clipping plane
    Vector3f background = scene.background;

    // camera
    Matrix4x4f L = scene.camera.cam_to_world; // lookAt + transform
    cameraPos = Vector3f{ L(0, 3), L(1, 3), L(2, 3) };
    cameraFront = Vector3f{ L(0, 2), L(1, 2), L(2, 2) };
    cameraUp = Vector3f{ L(0, 0), L(1, 0), L(2, 0) };



    // GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //mac os 
    #endif

    // Create Application Window
    GLFWwindow* window = glfwCreateWindow(width, height, "CSE 167 HW3", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    // GLAD: OpenGL function pointers (OS specific)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    // opengl state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);


    std::cout << "shaders" << std::endl;
    // SHADERS
    // --------
    const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aColor;\n"
        "out vec3 ourColor;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = projection * view * model * vec4(aPos, 1.0f);\n"
        "   ourColor = aColor;\n"
        "}\n";
    const char* fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec3 ourColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(ourColor, 1.0f);\n"
        "}\n";

    // define vertex shader
    unsigned int vertexShader;
    vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);

    // define fragment shader 
    // one output,vector of size 4, defines the final color output 
    unsigned int fragmentShader;
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    // Shader program: combine all shaders
    unsigned int shaderProgram;
    shaderProgram = createShaderProgram(vertexShader, fragmentShader);


    std::cout << "generate VAO,VBO,etc" << std::endl;
    // generate VAO/VBO/EBOs for each mesh
    int numMeshes = scene.meshes.size();
    std::vector<unsigned int> VAOs(numMeshes), VBOs_vertex(numMeshes), VBOs_color(numMeshes), VBOs_texture(numMeshes), EBOs(numMeshes);

    glGenVertexArrays(VAOs.size(), VAOs.data());
    glGenBuffers(VBOs_vertex.size(), VBOs_vertex.data());
    glGenBuffers(VBOs_color.size(), VBOs_color.data());
    glGenBuffers(VBOs_texture.size(), VBOs_texture.data());
    glGenBuffers(EBOs.size(), EBOs.data());

    for (int i = 0; i < numMeshes; i++) {
        glBindVertexArray(VAOs[i]);

        // position
        glBindBuffer(GL_ARRAY_BUFFER, VBOs_vertex[i]);
        glBufferData(GL_ARRAY_BUFFER,
            scene.meshes[i].vertices.size() * sizeof(Vector3f),
            scene.meshes[i].vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // color
        glBindBuffer(GL_ARRAY_BUFFER, VBOs_color[i]);
        glBufferData(GL_ARRAY_BUFFER,
            scene.meshes[i].vertex_colors.size() * sizeof(Vector3f),
            scene.meshes[i].vertex_colors.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);

        // texture
        glBindBuffer(GL_ARRAY_BUFFER, VBOs_texture[i]);
        glBufferData(GL_ARRAY_BUFFER,
            scene.meshes[i].uvs.size() * sizeof(Vector2f),
            scene.meshes[i].uvs.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(2);

        // faces
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            scene.meshes[i].faces.size() * sizeof(Vector3f),
            scene.meshes[i].faces.data(), GL_STATIC_DRAW);
    }
    
    std::cout << "textures" << std::endl;
    // textures
    // https://learnopengl.com/Getting-started/Textures
    // -------------------------
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // upcoming GL_TEXTURE_2D operations affect this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // GL_REPEAT: default wrapping method
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int tex_width, tex_height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load("../../scenes/hw3/teapot.png", &tex_width, &tex_height, &nrChannels, 0); 
    // ^^^ PATH ASSUMES EXECUTION FROM BUILD\DEBUG\ 
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture. Probably path related!!" << std::endl;
    }
    stbi_image_free(data);


    std::cout << "projection matrix" << std::endl;
    Matrix4x4f Projection = Matrix4x4::identity();
    Projection(0, 0) = 1.0f / ((float)a * (float)s); // aspect * scale
    Projection(1, 1) = 1.0f / (float)s; // scale
    Projection(2, 2) = -(float)z_far / ((float)z_far - (float)z_near);
    Projection(2, 3) = -(float)z_far * (float)z_near / ((float)z_far - (float)z_near);
    Projection(3, 2) = -1.0f;
    Projection(3, 3) = 0;


    std::cout << "render loop" << std::endl;
    // RENDER LOOP
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // handle input
        processInput(window);

        // set background color
        glClearColor(background.x, background.y, background.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // texture
        glBindTexture(GL_TEXTURE_2D, texture);
        glUseProgram(shaderProgram);

        // camera/view transformation
        L(0, 3) = cameraPos.x;
        L(1, 3) = cameraPos.y;
        L(2, 3) = cameraPos.z;
        Matrix4x4f V = inverse(L); // world_to_camera

        for (unsigned int i = 0; i < scene.meshes.size(); ++i) {
            TriangleMesh mesh = scene.meshes[i];

            Matrix4x4f M = mesh.model_matrix;

            unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
            unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
            unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, M.ptr());
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, V.ptr());
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, Projection.ptr());

            glBindVertexArray(VAOs[i]);
            glDrawElements(GL_TRIANGLES, mesh.faces.size() * sizeof(Vector3f) /*3*/, GL_UNSIGNED_INT, 0);
        }


        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    std::cout << "clean up" << std::endl;
    // clean up 
    glDeleteVertexArrays(VAOs.size(), VAOs.data());
    glDeleteBuffers(VBOs_vertex.size(), VBOs_vertex.data());
    glDeleteBuffers(VBOs_color.size(), VBOs_color.data());
    glDeleteBuffers(EBOs.size(), EBOs.data());


    std::cout << "end. return" << std::endl;
    return;

}

void hw_3_4(const std::vector<std::string> &params) {
    // HW 3.4: Render a scene with lighting
    if (params.size() == 0) {
        return;
    }
    // handle two 2D triangles first, \
    // add projection matrix, 
    // add view matrix, 
    // add model matrix, 
    // handle multiple triangle meshes, 
    // and finally add camera interaction.

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    // scene attributes
    // ----------------
    int width = scene.camera.resolution.x;
    int height = scene.camera.resolution.y;
    float a = float(width) / height; // aspect ratio
    float s = scene.camera.s; // scaling factor of the view frustrum
    z_near = scene.camera.z_near; // distance of the near clipping plane
    z_far = scene.camera.z_far; // distance of the far clipping plane
    Vector3f background = scene.background;

    // camera
    Matrix4x4f L = scene.camera.cam_to_world; // lookAt + transform
    cameraPos = Vector3f{ L(0, 3), L(1, 3), L(2, 3) };
    cameraFront = Vector3f{ L(0, 2), L(1, 2), L(2, 2) };
    cameraUp = Vector3f{ L(0, 0), L(1, 0), L(2, 0) };

    // GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //mac os 
    #endif

    // Create Application Window
    GLFWwindow* window = glfwCreateWindow(width, height, "CSE 167 HW3", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // GLAD: OpenGL function pointers (OS specific)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    // opengl state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);

    std::cout << "shaders" << std::endl;
    // SHADERS
    // --------
    const char* vertexShaderSource3_4 = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aColor;\n"
        "layout (location = 2) in vec2 aTexCoord;\n"
        "layout (location = 3) in vec3 aNormal;\n"
        "out vec3 ourColor;\n"
        "out vec3 FragPos;\n"
        "out vec3 Normal;\n"
        "out vec2 TexCoord;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "void main()\n"
        "{\n"
        "   FragPos = vec3(model * vec4(aPos, 1.0f));\n"
        "   Normal = mat3(transpose(inverse(model))) * aNormal;\n"
        "   gl_Position = projection * view * vec4(FragPos, 1.0f);\n"
        "   ourColor = aColor;\n"
        "   TexCoord = aTexCoord;\n"
        "}\n";
    const char* fragmentShaderSource3_4 = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec3 ourColor;\n"
        "in vec3 Normal;\n"
        "in vec3 FragPos;\n"
        "uniform vec3 viewPos;\n"
        "uniform vec3 lightColor;\n"
        "uniform vec3 lightPos;\n"
        "void main()\n"
        "{\n"
        "   float ambientStrength = 0.1;\n"
        "   vec3 ambient = ambientStrength * lightColor;\n"
        "   vec3 norm = normalize(Normal);\n"
        "   vec3 lightDir = normalize(lightPos);\n"
        "   float diff = max(dot(norm, lightDir), 0.0);\n"
        "   vec3 diffuse = diff * lightColor;\n"
        "   float specularStrength = 0.5;\n"
        "   vec3 viewDir = normalize(viewPos - FragPos);\n"
        "   vec3 reflectDir = reflect(-lightDir, norm);\n"
        "   float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);\n"
        "   vec3 specular = specularStrength * spec * lightColor;\n"
        "   vec3 result = (ambient + diffuse + specular) * ourColor;\n"
        "   FragColor = vec4(result, 1.0);\n"
        "}\n";  

    // vertex shader
    unsigned int vertexShader;
    vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource3_4);

    // fragment shader 
    // one output,vector of size 4, defines the final color output
    unsigned int fragmentShader;
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource3_4);

    // Create shader program
    unsigned int shaderProgram = createShaderProgram(vertexShader, fragmentShader);

    std::cout << "generate VAO,VBO,etc" << std::endl;
    // generate VAO/VBO/EBOs for each mesh
    int numMeshes = scene.meshes.size();    
    std::vector<unsigned int> VAOs(numMeshes), VBOs_vertex(numMeshes), VBOs_color(numMeshes), VBOs_texture(numMeshes), VBOs_normal(numMeshes), EBOs(numMeshes);


    glGenVertexArrays(VAOs.size(), VAOs.data());
    glGenBuffers(VBOs_vertex.size(), VBOs_vertex.data());
    glGenBuffers(VBOs_color.size(), VBOs_color.data());
    glGenBuffers(VBOs_texture.size(), VBOs_texture.data());
    glGenBuffers(VBOs_normal.size(), VBOs_normal.data());
    glGenBuffers(EBOs.size(), EBOs.data());

    for (int i = 0; i < numMeshes; i++) {
        glBindVertexArray(VAOs[i]);

        // position
        glBindBuffer(GL_ARRAY_BUFFER, VBOs_vertex[i]);
        glBufferData(GL_ARRAY_BUFFER,
            scene.meshes[i].vertices.size() * sizeof(Vector3f),
            scene.meshes[i].vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // color
        glBindBuffer(GL_ARRAY_BUFFER, VBOs_color[i]);
        glBufferData(GL_ARRAY_BUFFER,
            scene.meshes[i].vertex_colors.size() * sizeof(Vector3f),
            scene.meshes[i].vertex_colors.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);

        // texture
        glBindBuffer(GL_ARRAY_BUFFER, VBOs_texture[i]);
        glBufferData(GL_ARRAY_BUFFER,
            scene.meshes[i].uvs.size() * sizeof(Vector2f),
            scene.meshes[i].uvs.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(2);

        // normals
        glBindBuffer(GL_ARRAY_BUFFER, VBOs_normal[i]);
        glBufferData(GL_ARRAY_BUFFER,
            scene.meshes[i].vertex_normals.size() * sizeof(Vector3f),
            scene.meshes[i].vertex_normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(3);

        // faces
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            scene.meshes[i].faces.size() * sizeof(Vector3f),
            scene.meshes[i].faces.data(), GL_STATIC_DRAW);
    }

    std::cout << "textures" << std::endl;
    // textures
    // https://learnopengl.com/Getting-started/Textures
    // -------------------------
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // upcoming GL_TEXTURE_2D operations affect this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // GL_REPEAT: default wrapping method
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int tex_width, tex_height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load("../../scenes/hw3/teapot.png", &tex_width, &tex_height, &nrChannels, 0);
    // ^^^ PATH ASSUMES EXECUTION FROM BUILD\DEBUG\ 
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture. Probably path related!!" << std::endl;
    }
    stbi_image_free(data);


    std::cout << "projection matrix" << std::endl;
    Matrix4x4f Projection = Matrix4x4::identity();
    Projection(0, 0) = 1.0f / ((float)a * (float)s); // aspect * scale
    Projection(1, 1) = 1.0f / (float)s; // scale
    Projection(2, 2) = -(float)z_far / ((float)z_far - (float)z_near);
    Projection(2, 3) = -(float)z_far * (float)z_near / ((float)z_far - (float)z_near);
    Projection(3, 2) = -1.0f;
    Projection(3, 3) = 0;

    glUseProgram(shaderProgram);
    unsigned int projectionUnifLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionUnifLoc, 1, GL_FALSE, Projection.ptr());


    std::cout << "render loop" << std::endl;
    // RENDER LOOP
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // handle input
        processInput(window);

        // set background color
        glClearColor(background.x, background.y, background.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // texture
        glBindTexture(GL_TEXTURE_2D, texture);
        glUseProgram(shaderProgram);


        // set uniforms for lighting
        unsigned int viewPosUnifLoc = glGetUniformLocation(shaderProgram, "viewPos");
        unsigned int lightColorUnifLoc = glGetUniformLocation(shaderProgram, "lightColor");
        glUniform3fv(viewPosUnifLoc, 1, cameraPos.ptr());
        glUniform3f(lightColorUnifLoc, 1.0, 1.0, 1.0);

        // discover lighting
        lightPos.x = cos(glfwGetTime()); // move light in circle
        lightPos.y = 0.0f;
        lightPos.z = sin(glfwGetTime());
        //std::cout << "lighting x: " << lightPos.x << std::endl;
        //std::cout << "lighting y: " << lightPos.y << std::endl;
        //std::cout << "lighting z: " << lightPos.z << std::endl;
        // light animation or position (bonus)
        unsigned int lightPosUnifLoc = glGetUniformLocation(shaderProgram, "lightPos");
        glUniform3f(lightPosUnifLoc, lightPos.x, lightPos.y, lightPos.z); // ANIMATION
        //glUniform3f(lightPosUnifLoc, 1, 1, 1); // FOR FIXED LIGHTING
        

        // camera/view transformation
        L(0, 3) = cameraPos.x;
        L(1, 3) = cameraPos.y;
        L(2, 3) = cameraPos.z;
        Matrix4x4f V = inverse(L); // world_to_camera

        // make light rotate 

        // set view
        unsigned int viewUnifLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewUnifLoc, 1, GL_FALSE, V.ptr());

        for (unsigned int i = 0; i < scene.meshes.size(); ++i) {\
            // get current mesh
            TriangleMesh mesh = scene.meshes[i];

            // model matrix set uniform
            Matrix4x4f M = mesh.model_matrix;
            unsigned int modelUnifLoc = glGetUniformLocation(shaderProgram, "model");
            glUniformMatrix4fv(modelUnifLoc, 1, GL_FALSE, M.ptr());

            glBindVertexArray(VAOs[i]);
            glDrawElements(GL_TRIANGLES, mesh.faces.size() * sizeof(Vector3f) /*3*/, GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    std::cout << "clean up" << std::endl;
    // clean up 
    glDeleteVertexArrays(VAOs.size(), VAOs.data());
    glDeleteBuffers(VBOs_vertex.size(), VBOs_vertex.data());
    glDeleteBuffers(VBOs_color.size(), VBOs_color.data());
    glDeleteBuffers(EBOs.size(), EBOs.data());

    std::cout << "end. return" << std::endl;
    return;
}
