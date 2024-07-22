/* BROADCASTING USING FLOODING */

//Libraries
#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <queue>
#include <cmath>
#include <ctime>

int sequence_number = 0;

// Node structure, Edge structure, Packet structure
struct Node {
    float x, y;
    int id;
    std::vector<int> neighbors;  
    std::set<int> received_packets; 

    Node(int id, float x, float y) : id(id), x(x), y(y) {}

    bool is_clicked(float mx, float my) {
        float dx = x - mx;
        float dy = y - my;
        return (dx * dx + dy * dy) <= 100; 
    }
};

struct Edge {
    int node1;
    int node2;
};

struct Packet {
    std::string message;
    int sequence_number;
    int ttl;
    int sender; 
    int current_node;

    Packet(std::string message, int sequence_number, int ttl, int sender = -1, int current_node = -1)
        : message(message), sequence_number(sequence_number), ttl(ttl), sender(sender), current_node(current_node) {}
};

// Global variables 
std::vector<Node> nodes;
std::vector<Edge> edges;
std::set<std::pair<int, int>> edge_set;  
std::queue<Packet> packet_queue;
bool running = false;
std::vector<std::pair<int, int>> packet_transfers;

// Function prototypes
void flood_packet();
Packet create_packet(std::string message, int sequence_number, int ttl, int sender = -1, int current_node = -1);
void broadcast_message(int source_node_index, std::string message);
void draw_node(const Node& node);
void draw_edge(const Node& node1, const Node& node2);
void draw_packet_transfer(const Node& node1, const Node& node2);
void draw_network();
void display();
void init_network();
void timer(int);
void mouse(int button, int state, int x, int y);



void createRandomEdges(std::vector<Edge>& edges, std::vector<Node>& nodes, int max_edges_per_node) {
    std::vector<int> node_edges_count(nodes.size(), 0);
    int total_nodes = nodes.size();

    while (edges.size() < total_nodes * max_edges_per_node / 2) {
        int node1 = rand() % total_nodes;
        int node2 = rand() % total_nodes;

        if (node1 != node2 &&
            node_edges_count[node1] < max_edges_per_node &&
            node_edges_count[node2] < max_edges_per_node &&
            edge_set.find({std::min(node1, node2), std::max(node1, node2)}) == edge_set.end()) {

            Edge edge = { node1, node2 };
            edges.push_back(edge);
            edge_set.insert({std::min(node1, node2), std::max(node1, node2)});
            node_edges_count[node1]++;
            node_edges_count[node2]++;
        }
    }

    for (const auto& edge : edges) {
        nodes[edge.node1].neighbors.push_back(edge.node2);
        nodes[edge.node2].neighbors.push_back(edge.node1);
    }
}

void flood_packet() {
    if (packet_queue.empty()) {
        running = false;
        return;
    }

    Packet packet = packet_queue.front();
    packet_queue.pop();

    int current_node_index = packet.current_node;
    if (current_node_index == -1 || nodes[current_node_index].received_packets.count(packet.sequence_number)) return;

    nodes[current_node_index].received_packets.insert(packet.sequence_number);
    packet_transfers.push_back({ packet.sender, current_node_index });
    packet.ttl--;

    if (packet.ttl == 0) return;

    for (int neighbor_index : nodes[current_node_index].neighbors) {
        if (neighbor_index != packet.sender) {
            Packet new_packet = create_packet(packet.message, packet.sequence_number, packet.ttl, current_node_index, neighbor_index);
            packet_queue.push(new_packet);
        }
    }

    glutPostRedisplay();
    glutTimerFunc(100, timer, 0);  
}

Packet create_packet(std::string message, int sequence_number, int ttl, int sender, int current_node) {
    return Packet(message, sequence_number, ttl, sender, current_node);
}


void broadcast_message(int source_node_index, std::string message) {
    sequence_number++;
    Packet packet = create_packet(message, sequence_number, 5, -1, source_node_index);
    packet_queue.push(packet);
    running = true;
    packet_transfers.clear();
    glutTimerFunc(100, timer, 0);  
}


void draw_circle(float x, float y, float radius) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y); 
    for (int i = 0; i <= 100; ++i) {
        float angle = 2.0f * M_PI * i / 100;
        float dx = radius * cosf(angle);
        float dy = radius * sinf(angle);
        glVertex2f(x + dx, y + dy);
    }
    glEnd();
}

void draw_node(const Node& node) {
    glColor3f(0.0, 1.0, 0.0); 
    draw_circle(node.x, node.y, 10); 
}

void draw_edge(const Node& node1, const Node& node2) {
    glColor3f(1.0, 1.0, 1.0); 
    glBegin(GL_LINES);
    glVertex2f(node1.x, node1.y);
    glVertex2f(node2.x, node2.y);
    glEnd();
}

void draw_packet_transfer(const Node& node1, const Node& node2) {
    glColor3f(1.0, 0.0, 0.0); 
    glBegin(GL_LINES);
    glVertex2f(node1.x, node1.y);
    glVertex2f(node2.x, node2.y);
    glEnd();
}

void draw_network() {
    for (const Node& node : nodes) {
        draw_node(node);
        for (int neighbor_index : node.neighbors) {
            draw_edge(node, nodes[neighbor_index]);
        }
    }

    for (const auto& transfer : packet_transfers) {
        if (transfer.first >= 0 && transfer.second >= 0) {
            draw_packet_transfer(nodes[transfer.first], nodes[transfer.second]);
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw_network();
    glutSwapBuffers();
}

void init_network() {
    int num_nodes = 5;

    srand(static_cast<unsigned>(time(0)));

    nodes.clear(); 
    for (int i = 0; i < num_nodes; ++i) {
        float x = static_cast<float>(rand() % 800); 
        float y = static_cast<float>(rand() % 600); 
        nodes.emplace_back(i, x, y);
    }
    createRandomEdges(edges, nodes, 3);
}

void timer(int value) {
    flood_packet();
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 27) {  
        exit(0);
    }
    else if (key == 'r' || key == 'R') {  
        for (Node& node : nodes) {
            node.received_packets.clear();
        }
        while (!packet_queue.empty()) {
            packet_queue.pop();
        }
        broadcast_message(0, "Hello, Network!"); 
    }
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        float mx = static_cast<float>(x);
        float my = static_cast<float>(glutGet(GLUT_WINDOW_HEIGHT) - y);

        for (Node& node : nodes) {
            if (node.is_clicked(mx, my)) {
                for (Node& n : nodes) {
                    n.received_packets.clear();
                }
                while (!packet_queue.empty()) {
                    packet_queue.pop();
                }
                broadcast_message(node.id, "Hello, Network!"); 
                break;
            }
        }
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Flooding Algorithm Visualization");
    gluOrtho2D(0, 800, 0, 600);

    init_network();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMainLoop();

    return 0;
}
