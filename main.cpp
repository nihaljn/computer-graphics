/**
* @file
* @brief Driver file
*/
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include "circle.h"
#include "generator.h"
#include "graphics_engine.h"
#include "line.h"
#include "stb_image_write.h"
#include "tree.h"

int HEIGHT = 1000, WIDTH = 1300, DEPTH;

/**
 * @brief Utility function to log the tree representation and also save the image
 * generated by OpenGL.
 * 
 * @param fileName name of image file
 * @param log name of log file
 * @param treeRep level order binary tree representation
 */

void write_to_file(const char* fileName, const char* log, std::vector<int> &treeRep)
{
    std::ofstream outfile;
    outfile.open(log, std::ios::app);
    for (int i = 0; i < treeRep.size(); i++)
        outfile << treeRep[i] << ", ";
    outfile << "\n";

    unsigned char *data = (unsigned char*)malloc(WIDTH*HEIGHT*3);
    glReadPixels(0, 0, WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, (void*)data);
    stbi_write_jpg(fileName, WIDTH, HEIGHT, 3, data, 100);
    std::cout << "Wrote " << fileName << "\n";
    return;
}

int main(int argc, char* argv[])
{
    // max depth in argv[1]
    // fileName in argv[2]
    // log file in argv[3]

    std::string options = 
    "1. Random tree\n"
    "2. Complete tree\n"
    "3. Input tree (in level order)\n"
    "4. Exit\n"
    "Enter choice (1-4): ";
    int option = 0;

    // FOR BATCH COMMENT BELOW 2 LINES
    std::cout << options;
    std::cin >> option;

    std::vector<int> treeRep;
    switch(option)
    {
        case 1:
            std::cout << "Max depth of tree: ";
            std::cin >> DEPTH;
            HEIGHT = std::max((DEPTH + 1)*100, 500);
            HEIGHT = std::min(HEIGHT, 1000);
            WIDTH = std::max((1 << DEPTH)*20, 500);
            WIDTH = std::min(WIDTH, 1920);
            treeRep = random_tree(DEPTH);
            break;
        case 2:
            std::cout << "Max depth of tree: ";
            std::cin >> DEPTH;
            HEIGHT = std::max((DEPTH + 1)*100, 500);
            HEIGHT = std::min(HEIGHT, 1000);
            WIDTH = std::max((1 << DEPTH)*20, 500);
            WIDTH = std::min(WIDTH, 1920);
            treeRep = complete_binary_tree(DEPTH);
            break;
        case 3:
            int num;
            std::cout << "Number of nodes: ";
            std::cin >> num;
            treeRep.push_back(0);
            while (num--)
            {
                int nd; std::cin >> nd;
                treeRep.push_back(nd);
            }
            break;
        case 4:
            exit(0);
        default:
            break;
    }

    // FOR BATCH
    // DEPTH = atoi(argv[1]);
    // treeRep = complete_binary_tree(depth);
    // treeRep = {0, 1, 1, 0};
    // treeRep = random_tree(DEPTH);
    HEIGHT = std::max((DEPTH + 1)*100, 500);
    HEIGHT = std::min(HEIGHT, 1000);
    WIDTH = std::max((1 << DEPTH)*20, 500);
    WIDTH = std::min(WIDTH, 1920);

    GraphicsEngine engine = GraphicsEngine();

    Tree tree = Tree(treeRep);
    std::vector<Circle> nodes = tree.get_nodes();
    std::vector<Line> edges = tree.get_edges();
    for (int i = 0; i < nodes.size(); i++)
        engine.load_circle(nodes[i]);
    for (int i = 0; i < edges.size(); i++)
        engine.load_line(edges[i]);

    // render loop
    while (!engine.close_window())
    {
        // process all input before updating frame
        engine.process_input();
        // set color of window
        engine.set_background_color(0.1f, 0.1f, 0.1f, 0.0f);
        // Drawing happens here.
        engine.draw();
        // write_to_file(argv[2], argv[3], treeRep);
        // break;
        // update window
        engine.update_window();
        // sleep(3);
    }

    engine.terminate();
    std::cout << "Ending program...\n";

    return 0;
}