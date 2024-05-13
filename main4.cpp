#include <SFML/Graphics.hpp>
#include <bits/stdc++.h>
using namespace std;
// function to add two vectors for equal size
vector<int> vectorAdd(vector<int> v1, vector<int> v2);




class vertex{
    public:
        int x, y;
        bool operator==(vertex const& v){
            return (v.x==this->x && v.y==this->y);
        }
        bool operator!=(vertex const& v){
            return (v.x==this->x && v.y==this->y);
        }
        vertex(int x=0, int y=0){
            this->x = x;
            this->y = y;
        }
};

class label;

class state{
    public:
        vertex v;
        vector<vertex> mo;
        vector<label*> frontier;
        state(){
        }
        state(vertex v, vector<vertex> mo){
            this->v = v;
            this->mo = mo;
        }
        bool operator==(state const s){
            for(int i=0; i<this->mo.size(); i++){
                bool found=false;
                for(int j=0; j<s.mo.size(); j++){
                    if(this->mo[i] != s.mo[j]) found=true;
                }
                if(!found) return false;
            }
            return (s.v.x == this->v.x && s.v.y == this->v.y);
        }


        vector<state> getSuccessors(vector<vertex> &movObj, vector<vertex> &walls, int mapHeight, int mapWidth){
            int xmoves[4] = {1, 0, -1, 0};
            int ymoves[4] = {0, -1, 0, 1};
            int linedUpObjX[4] = {2, 0, -2, 0};
            int linedUpObjY[4] = {0, -2, 0, 2};

            vector<state> successors;
            for(int i=0; i<4; i++){
                bool hasObj = false;
                bool hasLinedUpObj = false;
                bool hasWall = false;

                if(this->v.x+xmoves[i] <=0 || this->v.x+xmoves[i] > mapWidth || this->v.y+ymoves[i]<=0 || this->v.y+ymoves[i]>mapHeight) continue;
                
                for(int j=0; j<this->mo.size(); j++){
                    // has movable obj right in front?
                    if(this->mo[j].x == (this->v.x+xmoves[i]) && this->mo[j].y == (this->v.y+ymoves[i])){
                        hasObj = true;
                    }
                    
                    // 2 steps ahead is out of bound or has a movable object considered as lined up object
                    if((this->mo[j].x == (this->v.x+linedUpObjX[i]) && this->mo[j].y == (this->v.y+linedUpObjY[i])) \
                    || this->v.x+linedUpObjX[i]<=0 || this->v.x+linedUpObjX[i] > mapWidth || this->v.y+linedUpObjY[i]<=0 || this->v.y+linedUpObjY[i]>mapHeight){
                        hasLinedUpObj = true;
                    }
                }

                for(int j=0; j<walls.size(); j++){
                    if(walls[j].x == (this->v.x+xmoves[i]) && walls[j].y == (this->v.y+ymoves[i])){
                        hasWall = true;
                    }

                    // if has a wall 2 steps ahead, consider as a lined up object
                    if((walls[j].x == (this->v.x+linedUpObjX[i]) && walls[j].y == (this->v.y+linedUpObjY[i])))
                        hasLinedUpObj = true;
                }
                // if there is a wall or lined up obj, continue

                if((hasObj && hasLinedUpObj) || hasWall) continue;

                // now there is no wall or lined up object, but there can be a movable object in front
                if(!hasObj){
                    vertex vv = vertex(this->v.x+xmoves[i], this->v.y+ymoves[i]);
                    vector<vertex> new_mo;
                    for(int j=0; j<this->mo.size(); j++){
                        new_mo.push_back(this->mo[j]);
                    }
                    state newSuccessor = state(vv, new_mo);
                    successors.push_back(newSuccessor);
                }
                if(hasObj){
                    vertex vv = vertex(this->v.x+xmoves[i], this->v.y+ymoves[i]);
                    vector<vertex> new_mo;
                    for(int j=0; j<this->mo.size(); j++){
                        if(this->mo[j].x == (this->v.x+xmoves[i]) && this->mo[j].y == (this->v.y+ymoves[i])){
                            new_mo.push_back(this->mo[j]);
                            new_mo.back().x = this->v.x+linedUpObjX[i];
                            new_mo.back().y = this->v.y+linedUpObjY[i];
                            continue;
                        }
                        new_mo.push_back(this->mo[j]);
                    }
                    state newSuccessor = state(vv, new_mo);
                    successors.push_back(newSuccessor);
                }
            }
            return successors;
        }
};


class label{
    public:
        state s;
        vector<int> cost;
        label *p;
        vector<vertex> path;

        label(state s, vector<int> cost){
            this->cost = cost;
            this->s = s;
            this->s.frontier = s.frontier;
            this->s.mo = s.mo;
            this->s.v = s.v;
        }
        void parent(label *p){
            this->p = p;
        }
};




int heuristic(state &s, vertex &d){
    return (abs(s.v.x-d.x) + abs(s.v.y-d.y))/2;
}



// custom compare function for priority queue A.K.A. OPEN set
class Compare{
    public:
        bool operator()(pair<vector<int>, label> below, pair<vector<int>, label> above){
            if(below.first[0] > above.first[0])
                return true;
            else if(below.first[0]==above.first[0] && below.first[1]>above.first[1])
                return true;
            else return false;
        }
};


bool isPrunByFront(label &l, vector<state> &frontier_set){
    if(frontier_set.size()==0) return false;
    state curr_state = l.s;
    int ctr=0;
    for(ctr=0; ctr<frontier_set.size(); ctr++){
        if(l.s == frontier_set[ctr]){
            break;
        }
    }

    if(ctr == frontier_set.size()){
        return false;
    }

    
    vector<int> cost = l.cost;
    for(int i=0; i<frontier_set[ctr].frontier.size(); i++){
        vector<int> setCost = frontier_set[ctr].frontier[i]->cost;

        bool result = true;
        bool equalFound = false;
        bool largerFound = false;
        bool smallerFound = false;
        for(int j=0; j<cost.size(); j++){
            if(setCost[j]<cost[j]) smallerFound = true;
            else if(setCost[j]==cost[j]) equalFound = true;
            else largerFound = true;
        }
        if((smallerFound && (!largerFound)) || (equalFound && (!largerFound))) return true;
    }
    return false;
}

bool isPrunByResour(label l, vector<int> &resLimits){
    vector<int> cost = l.cost;
    for(int i=0; i<resLimits.size(); i++){
        if(cost[i]>resLimits[i])
            return true;
    }
    return false;
}


void FilterAndAddFront(label &l, vector<state> &frontier_set){
    int i=0;
    for(i=0; i<frontier_set.size(); i++){
        if(l.s == frontier_set[i]){
            cout << "If condition executed\n";
            frontier_set[i].frontier.push_back(&l);
            return;
        }
    }
    frontier_set.push_back(l.s);
    frontier_set.back().frontier.push_back(&l);
}



vector<int> cost(state origin, state successor){
    bool pushed = false;
    for(int i=0; i<origin.mo.size(); i++){
        if(successor.v.x == origin.mo[i].x && successor.v.y == origin.mo[i].y){
            pushed = true;
        }
    }
    vector<int> cost;
    cost.push_back(1);
    if(pushed) cost.push_back(1);
    else cost.push_back(0);
    return cost;
}



void constructEnv(int &height, int &width, vector<vertex> &movObj, vector<vertex> &walls, vertex &start, vertex &dest, vector<int> &resLimits);



vector<vertex> rca_star(int mapHeight, int mapWidth, vector<vertex> &movObj, vector<vertex> &walls, vertex start, vertex dest, vector<int> &resLimits){
    state startState = state(start, movObj);
    vector<int> startCost(2, 0);
    label startLabel = label(startState, startCost);
    startLabel.path.push_back(startState.v);
    vector<int> fval = vectorAdd(startLabel.cost, {heuristic(startState, dest), 0});
    
    priority_queue<pair<vector<int>, label>, vector<pair<vector<int>, label>>, Compare> open;
    open.push({fval, startLabel});
    vector<state> frontier_set;
    
    while(!open.empty()){
        label l = open.top().second; open.pop();
        if(isPrunByFront(l, frontier_set) || isPrunByResour(l, resLimits)){
            continue;
        }
        FilterAndAddFront(l, frontier_set);

        if(l.s.v==dest){
            movObj = l.s.mo;
            return l.path;
        }

        vector<state> successors = l.s.getSuccessors(movObj, walls, mapHeight, mapWidth);
        
        for(int i=0; i<successors.size(); i++){
            label newLabel = label(successors[i], vectorAdd(l.cost, cost(l.s, successors[i])));

            newLabel.path = l.path;
            newLabel.path.push_back(newLabel.s.v);
            
            fval = vectorAdd(newLabel.cost, {heuristic(newLabel.s, dest), 0});

            if(isPrunByFront(newLabel, frontier_set) || isPrunByResour(newLabel, resLimits))
                continue;
            open.push({fval, newLabel});
            label p = open.top().second;
        }
        
    }
    vector<vertex> vv;
    return vv;
}


void initSquares(vector<sf::RectangleShape> squares[], int mapHeight, int mapWidth){
    for(int i=0; i<mapWidth; i++){
        for (int j = 0; j < mapHeight; j++)
        {
            sf::RectangleShape rect(sf::Vector2f(196, 196));
            rect.setPosition(200*i+4, mapHeight*200-196-200*j);
            rect.setFillColor(sf::Color(192, 192, 192));
            rect.setOutlineThickness(4);
            rect.setOutlineColor(sf::Color(0, 0, 0));
            squares[i].push_back(rect);
        }
    }
}

void processArgs(vector<int> &resLimits, char *argv[]){
    resLimits.push_back(atoi(argv[2]));
    resLimits.push_back(atoi(argv[4]));
}

int main(int argc, char *argv[])
{
    int mapHeight, mapWidth;
    vector<vertex> movObj;
    vector<vertex> walls;
    vertex start = vertex();
    vertex dest = vertex();
    vector<int> resLimits;
    processArgs(resLimits, argv);
    cout << "passed";
    constructEnv(mapHeight, mapWidth, movObj, walls, start, dest, resLimits);
    vector<vertex> prevMovObj = movObj;
    cout << resLimits[0] << ' ' << resLimits[1] << " res limits" << endl;
    vector<vertex> path = rca_star(mapHeight, mapWidth, movObj, walls, start, dest, resLimits);
    
    cout << "FINAL PATH\n";
    for(int i=0; i<path.size(); i++){
        cout << path[i].x << ' ' << path[i].y << endl;
    }
    if(path.size()==0){
        cout << "\n------------\nPATH NOT FOUND\n------------\n\n";
    }



    // GRAPHICS


    // initialize a window
    sf::RenderWindow window(sf::VideoMode(mapWidth*200, mapHeight*200+200), "Path Planning", sf::Style::Close);
    // fps the same with monitor's fps
    window.setVerticalSyncEnabled(true);

    vector<sf::RectangleShape> squares[mapWidth];

    sf::RectangleShape button(sf::Vector2f(200, 100));
            button.setPosition(mapWidth*100-98, mapHeight*200+50);
            button.setFillColor(sf::Color(0, 0, 0));
            button.setOutlineThickness(4);
            button.setOutlineColor(sf::Color(0, 0, 0));


    initSquares(squares, mapHeight, mapWidth);
    

    bool drawThePath = false;
    while (window.isOpen())
    {
        // event handling
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed)
            {
                if(event.mouseButton.button == sf::Mouse::Left)
                {
                    std::cout << "the left button was pressed" << std::endl;
                    std::cout << "mouse x: " << event.mouseButton.x << std::endl;
                    std::cout << "mouse y: " << event.mouseButton.y << std::endl;
                    if(event.mouseButton.x>mapWidth*100-98 && event.mouseButton.x<mapWidth*100-98+200 && event.mouseButton.y>mapHeight*200+50 && event.mouseButton.y<mapHeight*200+150)
                        drawThePath = true;
                }
                if (event.mouseButton.button == sf::Mouse::Right)
                {
                    std::cout << "the right button was pressed" << std::endl;
                    std::cout << "mouse x: " << event.mouseButton.x << std::endl;
                    std::cout << "mouse y: " << event.mouseButton.y << std::endl;
                }
            }
        }

        // draw
        for(int i=0; i<walls.size(); i++){
            squares[walls[i].x-1][walls[i].y-1].setFillColor(sf::Color(96, 96, 96));
        }
        if(drawThePath==true){
            for(int i=0; i<movObj.size(); i++){
                squares[prevMovObj[i].x-1][prevMovObj[i].y-1].setFillColor(sf::Color(192, 192, 192));
                squares[movObj[i].x-1][movObj[i].y-1].setFillColor(sf::Color(172, 90, 0));
            }
            for(int i=0; i<path.size(); i++){
                squares[path[i].x-1][path[i].y-1].setFillColor(sf::Color(144, 238, 144));
            }
        }
        else{
            for(int i=0; i<prevMovObj.size(); i++){
                squares[prevMovObj[i].x-1][prevMovObj[i].y-1].setFillColor(sf::Color(172, 90, 0));
            }
        }
        squares[start.x-1][start.y-1].setFillColor(sf::Color(30, 80, 200));
        squares[dest.x-1][dest.y-1].setFillColor(sf::Color(0, 128, 0));
        window.clear(sf::Color::White);
        for(int i=0; i<mapWidth; i++){
            for(int j=0; j<mapHeight; j++){
                window.draw(squares[i][j]);
            }
        }
        window.draw(button);
        window.display();
    }

    return 0;
}




void constructEnv(int &height, int &width, vector<vertex> &movObj, vector<vertex> &walls, vertex &start, vertex &dest, vector<int> &resLimits){
    height = 5;
    width = 5;
    movObj.push_back(vertex(1, 3));
    movObj.push_back(vertex(2, 4));
    movObj.push_back(vertex(3, 3));
    walls.push_back(vertex(2, 2));
    walls.push_back(vertex(4, 2));
    walls.push_back(vertex(4, 4));
    walls.push_back(vertex(5, 4));
    start.x = 1;
    start.y = 1;
    dest.x = 5;
    dest.y = 5;
    return;
}


vector<int> vectorAdd(vector<int> v1, vector<int> v2){
    vector<int> v;
    for(int i=0; i<v1.size(); i++){
        v.push_back(v1[i]+v2[i]);
    }
    return v;
}

