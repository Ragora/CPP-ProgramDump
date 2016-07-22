/**
 *  @file main.cpp
 *  @author Robert MacGregor
 */

#include <stack>    // std::stack
#include <fstream>  // std::ifstream
#include <iostream>
#include <cstring>
#include <time.h>   // clock(), clock_t
#include <vector>   // std::vector

using namespace std;

#define DIRECTION_DOWN Maze::Vector2D(0, 1)
#define DIRECTION_UP Maze::Vector2D(0, -1)
#define DIRECTION_LEFT Maze::Vector2D(-1, 0)
#define DIRECTION_RIGHT Maze::Vector2D(1, 0)

#define CLEAR_COMMAND "clear"

namespace Maze
{
    /**
     *  @
     */
    struct Vector2D
    {
        size_t column;
        size_t row;

        Vector2D(int column = 0, int row = 0) : column(column), row(row) { }

        Vector2D operator +(const Vector2D &rhs)
        {
            return Vector2D(column + rhs.column, row + rhs.row);
        }

        Vector2D operator -(const Vector2D& rhs)
        {
            return Vector2D(column - rhs.column, row - rhs.row);
        }

        Vector2D& operator +=(const Vector2D& rhs)
        {
            column += rhs.column;
            row += rhs.row;
            return *this;
        }

        bool operator ==(const Vector2D& rhs)
        {
            return (column == rhs.column) && (row == rhs.row);
        }

        bool operator !=(const Vector2D& rhs)
        {
            return !operator==(rhs);
        }

        friend ostream& operator <<(ostream &stream, const Vector2D& input)
        {
            stream << input.column << "," << input.row;
            return stream;
        }
    };

    struct Bot
    {
        Maze::Vector2D position;

        enum Direction
        {
            DOWN = 0,
            UP = 1,
            LEFT = 2,
            RIGHT = 3,
        };

        Bot::Direction direction;

        static Maze::Vector2D directionToVector(const Bot::Direction& direction)
        {
            switch (direction)
            {
                case Bot::Direction::DOWN:
                    return DIRECTION_DOWN;
                case Bot::Direction::UP:
                    return DIRECTION_UP;
                case Bot::Direction::LEFT:
                    return DIRECTION_LEFT;
                case Bot::Direction::RIGHT:
                    return DIRECTION_RIGHT;
            }

            return Maze::Vector2D(0, 0);
        }
    };

    struct NodeInformation
    {
        Maze::Vector2D position;

        bool left;
        bool right;
        bool up;
        bool down;

        bool wentLeft;
        bool wentRight;
        bool wentUp;
        bool wentDown;

        NodeInformation(void) : position(Maze::Vector2D(0, 0)),
        left(false), right(false), up(false), down(false), wentLeft(false),
        wentRight(false), wentUp(false), wentDown(false) { }
    };

    struct ExitInformation
    {
        Maze::Vector2D position;
        Bot::Direction direction;
    };

    static bool **sMaze = NULL;
    static char **sVideo = NULL;

    static size_t sRowCount = 0;
    static size_t sColumnCount = 0;

    static void print(const bool& clearConsole = false)
    {
        if (clearConsole)
            system(CLEAR_COMMAND);

        for (size_t row = 0; row < sRowCount; row++)
        {
            for (size_t column = 0; column < sColumnCount; column++)
                cout << sVideo[row][column];

                //if (robot.column >= 0 && robot.row >= 0 && row == robot.row && column == robot.column)
                //    cout << 'B';
                //else
                //    cout << (sMaze[row][column] ? 'X' : ' ');

            cout << endl;
        }
    }

    static void destroy(void)
    {
        // Destroy the maze
        for (size_t row = 0; row < sRowCount; row++)
        {
            delete sMaze[row];
            delete sVideo[row];
        }

        delete[] sMaze;
        delete[] sVideo;
    }

    /**
     *  @brief A helper method that loads the maze data from data into memory and writes the
     *  video buffer with all data.
     */
    vector<ExitInformation> load(const string &data)
    {
        vector<ExitInformation> results;

        // Allocate the maze
        sMaze = new bool*[sRowCount];
        sVideo = new char*[sRowCount];

        for (size_t row = 0; row < sRowCount; row++)
        {
            sMaze[row] = new bool(false);
            sVideo[row] = new char(' ');
        }

        // Put the maze data in our maze
        for (size_t iteration = 0; iteration < data.length(); iteration++)
        {
            const size_t row = iteration / sColumnCount;
            const size_t column = iteration % sColumnCount;

            // User specified exit?
            if (data[iteration] == 'O')
            {
                sMaze[row][column] = false;
                sVideo[row][column] = 'O';

                ExitInformation exit;
                exit.position = Vector2D(column, row);

                results.insert(results.end(), exit);
                continue;
            }

            sMaze[row][column] = (data[iteration] == 'X' ? true : false);
        }

        // Populate the video with the wall data
        for (size_t row = 0; row < sRowCount; row++)
            for (size_t column = 0; column < sColumnCount; column++)
            {
                if (sVideo[row][column] == 'O')
                    continue;

                sVideo[row][column] = sMaze[row][column] ? 'X' : ' ';
            }

        return results;
    }

    /**
     *  @brief A helper method that finds all the possible entry and exit points in the maze.
     *  @return An std::vector<ExitInformation> containing all known entry and exit points in
     *  the maze.
     */
    vector<ExitInformation> findEntryPoints(void)
    {
        vector<ExitInformation> result;

        for (size_t row = 0; row < sRowCount; row++)
                // If we're the top or bottom, scan the entire row
                if (row == 0 || row == sRowCount - 1)
                    for (size_t column = 0; column < sColumnCount; column++)
                        // We found a viable start position.
                        if (!sMaze[row][column])
                        {
                            ExitInformation exit;
                            exit.position = Maze::Vector2D(column, row);

                            // Start downwards if we're the top row
                            if (row == 0)
                                exit.direction = Bot::DOWN;
                            // Go upwards otherwise
                            else
                                exit.direction = Bot::UP;

                            result.insert(result.end(), exit);

                            break;
                        }
                // Otherwise, we're in the middle, which means that we only care about the left and right ends
                else
                    if (!sMaze[row][0])
                    {
                        ExitInformation exit;
                        exit.position = Maze::Vector2D(0, row);

                        // We're starting on the left, so start moving right
                        exit.direction = Bot::RIGHT;

                        result.insert(result.end(), exit);
                        break;
                    }
                    else if (!sMaze[row][sColumnCount - 1])
                    {
                        ExitInformation exit;
                        exit.position = Maze::Vector2D(sColumnCount - 1, row);

                        // We're starting on the right, so start moving left
                        exit.direction = Bot::LEFT;

                        result.insert(result.end(), exit);
                        break;
                    }

        return result;
    }
} // End Namespace Maze

/**
 *  @brief The main entry point of our program.
 *  @param argc The number of space delineated command line parameters in
 *  argv.
 *  @param argv An array of pointers to space delineated strings
 *  from the command line.
 *  @return An integer representing the return status.
 */
int main(int argc, char* argv[])
{
    // Ensure that we have two parameters.
    if (argc != 2)
    {
        cout << "Usage: " << argv[0] << " <Maze File> " << endl;
        return 0;
    }

    ifstream input(argv[1]);

    // File exists?
    if (!input)
    {
        cout << "Error: No such file '" << argv[1] << "'" << endl;
        input.close();

        return -1;
    }

    // Yes, load the maze data
    string fileBuffer;

    char currentLine[256];
    while (input)
    {
        input.getline(currentLine, 256);

        const size_t currentLineLength = strlen(currentLine);

        if (currentLineLength == 0)
            continue;

        if (Maze::sColumnCount == 0)
            Maze::sColumnCount = currentLineLength;
        else if (Maze::sColumnCount != currentLineLength)
        {
            input.close();
            cout << "Error: Inconsistent maze proportions! " << endl;
            return -2;
        }

        fileBuffer += currentLine;

        Maze::sRowCount++;
    }

    vector<Maze::ExitInformation> userExits = Maze::load(fileBuffer);

    // Scan the outsides of the maze for a viable starting point.
    Maze::Bot bot;

    vector<Maze::ExitInformation> entrances = Maze::findEntryPoints();

    if (entrances.size() == 0 || (entrances.size() == 1 && userExits.size() == 0))
    {
        Maze::destroy();
        cout << "Error: The maze must have at least two entrances/exits on the exterior sides!" << endl;
        return -2;
    }

    // Pick the first available entrance
    bot.position = entrances[0].position;
    bot.direction = entrances[0].direction;

    entrances.erase(entrances.begin());

    // Insert the user specified exists
    for (auto it = userExits.begin(); it != userExits.end(); it++)
        entrances.insert(entrances.end(), *it);

    // Before we start, put the bot in its initial position in the video buffer
    Maze::sVideo[bot.position.row][bot.position.column] = 'B';

    // Print the maze dimensions and allocate the memory
    cout << "Going to solve a " << Maze::sColumnCount << "x" << Maze::sRowCount << " maze: " << endl;

    // Print the maze data
    Maze::print();

    stack<Maze::NodeInformation> visitedLocations;
    clock_t last = clock();
    while (true)
    {
        clock_t now = clock();
        clock_t difference = now - last;

        // Step the bot about once every second
        if ((difference / CLOCKS_PER_SEC) >= 1)
        {
            last = now;

            // First, have we solved the maze?
            for (auto it = entrances.begin(); it != entrances.end(); it++)
                if ((*it).position == bot.position)
                {
                    // We write the path data to the video and reprint one last time
                    while (!visitedLocations.empty())
                    {
                        Maze::NodeInformation& current = visitedLocations.top();
                        Maze::sVideo[current.position.row][current.position.column] = '*';

                        cout << current.position << endl;

                        visitedLocations.pop();
                    }

                    // Draw the maze with our path data
                    Maze::print(false);

                    cout << "Bot has found the exit!" << endl;
                    cout << "The path taken is desiginated by '*' " << endl;

                    // Destroy the allocated memory and return.
                    Maze::destroy();
                    return 0;
                }

            Maze::NodeInformation currentNode;
            currentNode.position = bot.position;

            // Can we go up?
            if (bot.position.row != 0)
                currentNode.up = Maze::sMaze[bot.position.row - 1][bot.position.column] ? false : true;

            // Down?
            if (bot.position.row != Maze::sRowCount - 1)
                currentNode.down = Maze::sMaze[bot.position.row + 1][bot.position.column] ? false : true;

            // Left?
            if (bot.position.column != 0)
                currentNode.left = Maze::sMaze[bot.position.row][bot.position.column - 1] ? false : true;

            // ... and right?
            if (bot.position.column != Maze::sColumnCount - 1)
                currentNode.right = Maze::sMaze[bot.position.row][bot.position.column + 1] ? false : true;

            // What is our direction?
            Maze::Vector2D direction;
            switch (bot.direction)
            {
                case Maze::Bot::DOWN:
                {
                    direction = DIRECTION_DOWN;
                    break;
                }
                case Maze::Bot::UP:
                {
                    direction = DIRECTION_UP;
                    break;
                }
                case Maze::Bot::LEFT:
                {
                    direction = DIRECTION_LEFT;
                    break;
                }
                case Maze::Bot::RIGHT:
                {
                    direction = DIRECTION_RIGHT;
                    break;
                }
            }

            // Can we move in the direction we're currently moving?
            Maze::Vector2D lookAhead = bot.position + direction;

            // Can't move if our next desired position is a wall
            if (Maze::sMaze[lookAhead.row][lookAhead.column])
            {
                bool foundBranch = false;

                // Is there any other direction we can possibly go to (aside from the opposite of our current direction)
                switch (bot.direction)
                {
                    case Maze::Bot::DOWN:
                    case Maze::Bot::UP:
                    {
                        currentNode.wentDown = true;
                        currentNode.wentUp = true;

                        if (currentNode.left && !currentNode.wentLeft)
                        {
                            currentNode.wentLeft = true;
                            bot.direction = Maze::Bot::LEFT;
                            direction = DIRECTION_LEFT;
                            foundBranch = true;
                        }
                        else if (currentNode.right && !currentNode.wentRight)
                        {
                            bot.direction = Maze::Bot::RIGHT;
                            direction = DIRECTION_RIGHT;
                            foundBranch = true;
                            currentNode.wentRight = true;
                        }

                        break;
                    }
                    case Maze::Bot::LEFT:
                    case Maze::Bot::RIGHT:
                    {
                        currentNode.wentLeft = true;
                        currentNode.wentRight = true;
                        if (currentNode.up && !currentNode.wentUp)
                        {

                            bot.direction = Maze::Bot::UP;
                            direction = DIRECTION_UP;
                            foundBranch = true;
                        }
                        else if (currentNode.down && !currentNode.wentDown)
                        {
                            bot.direction = Maze::Bot::DOWN;
                            direction = DIRECTION_DOWN;
                            foundBranch = true;
                        }

                        break;
                    }
                }

                if (!foundBranch)
                {
                    // No, we must keep stepping back until we hit a node we can make another choice on
                    last = clock();

                    while (!visitedLocations.empty() && !foundBranch)
                    {
                        now = clock();
                        difference = now - last;

                        if ((difference / CLOCKS_PER_SEC) >= 1)
                        {
                            last = now;

                            // Traverse back a step
                            Maze::NodeInformation currentStep = visitedLocations.top();
                            visitedLocations.pop();
                            Maze::NodeInformation &nextStep = visitedLocations.top();

                            // Can we move in any direction we have not already visited?
                            if (currentStep.down && !currentStep.wentDown && nextStep.position != currentStep.position + DIRECTION_DOWN)
                            {
                                foundBranch = true;
                                bot.direction = Maze::Bot::DOWN;
                                currentStep.wentDown = true;
                            }
                            else if (currentStep.up && !currentStep.wentUp && nextStep.position != currentStep.position + DIRECTION_UP)
                            {
                                foundBranch = true;
                                bot.direction = Maze::Bot::UP;
                                currentStep.wentUp = true;
                            }
                            else if (currentStep.left && !currentStep.wentLeft && nextStep.position != currentStep.position + DIRECTION_LEFT)
                            {
                                foundBranch = true;
                                bot.direction = Maze::Bot::LEFT;
                                currentStep.wentLeft = true;
                            }
                            else if (currentStep.right && !currentStep.wentRight && nextStep.position != currentStep.position + DIRECTION_RIGHT)
                            {
                                foundBranch = true;
                                bot.direction = Maze::Bot::RIGHT;
                                currentStep.wentRight = true;
                            }

                            // When we alter the bot position in step back, clear it from the buffer and redraw
                            Maze::sVideo[bot.position.row][bot.position.column] = ' ';
                            bot.position = currentStep.position;
                            Maze::sVideo[bot.position.row][bot.position.column] = 'B';

                            // Destroy whatever is at the top of the stack.
                            if (foundBranch)
                                visitedLocations.push(currentStep);

                            // Redraw
                            Maze::print(true);
                            cout << "Position: " << bot.position << endl;
                            cout << "The bot is currently backtracking to an unused branch" << endl;
                        }
                    }

                    if (!foundBranch)
                    {
                        Maze::destroy();
                        cout << "Error: Bot got stuck! No solution. " << endl;
                        return -3;
                    }

                    continue;
                }
            }

            // Take our bot out of its last slot
            Maze::sVideo[bot.position.row][bot.position.column] = ' ';

            // Update the bot position
            bot.position += direction;

            // Redraw the bot
            Maze::sVideo[bot.position.row][bot.position.column] = 'B';

            // Update the node information to reflect that we have moved this direction on the last move
            switch (bot.direction)
            {
                case Maze::Bot::DOWN:
                    currentNode.wentDown = true;
                    break;
                case Maze::Bot::UP:
                    currentNode.wentUp = true;
                    break;
                case Maze::Bot::LEFT:
                    currentNode.wentLeft = true;
                    break;
                case Maze::Bot::RIGHT:
                    currentNode.wentRight = true;
                    break;
            }

            // Push the current position to the stack
            visitedLocations.push(currentNode);

            // Only redraw when we actually made a change.
            Maze::print(true);
            cout << "Position: " << bot.position << endl;
        }
    }

    Maze::destroy();

    cout << "Maze destroyed. Have a nice day! " << endl;

    return 0;
}
