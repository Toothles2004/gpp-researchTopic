# Flowfield pathfinding
## Introduction
I am a student in DAE and got the assignment to choose a research topic.
For this i chose to delve deeper into Influence maps and flowfield pathfinding based on Elijah Emerson's article GameAIPro crowd pathfinding.
The project is written in C++ and made using the engine written by DAE teachers.

## Example
![InfluencePathfinding](https://github.com/Howest-DAE-GD/gpp-researchtopic-Toothles2004/assets/118281295/60b30d7d-eb41-40c0-b07c-a1f4c4857c5f)

## What are Flowfields and FlowField pathfinding?
Flowfields are a pathfinding technique used in game development and robotics. 
They involve creating a grid of vectors that represent the direction or flow of movement at each point on the grid. 
These vectors guide entities through the environment efficiently, especially in scenarios involving complex and dynamic paths.

FlowField Pathfinding is the process of navigating through a space using a precomputed flowfield. 
Each cell in the grid contains a vector indicating the direction to move. 
By following these vectors, entities can navigate around obstacles and reach their destination efficiently.

## Benefits of FlowFields
Because paths are pre calculated flowfield pathfinding is computationally very efficient, it reduces the need for real-time path calculations.
The flowfield can adapt to changes in the environment because they adapt without needing to recalculate the entire path.
It is very suitable for a large number of agents because the technique scales well without much loss of performance.
The vectors make the agents move smoothly along precomputed paths making for a more natural and realistic movement pattern.

## Implementation

### Creating the map
First we program a grid system that that stores information in each cell.
We can add impassable terrain to the grid, this has a value so high that normal terrain can never have (INT_MAX).

<img width="285" alt="Screenshot_2" src="https://github.com/Howest-DAE-GD/gpp-researchtopic-Toothles2004/assets/118281295/f95f39c3-38a9-4bef-8de0-19ffdc3c4c76">

### Dijkstra algorithm
We use the Dijkstra algorithm to calculate the easiest path to the target

1) We set all the values in the grid to 0, a boolean with visited to false and the velocities to 0.
2) We set all of the cell values with impassable terrain to INT_MAX and the bool to true.
(this is an extra check to ensure that we don't need to take obstacles into account when calculating)
3) Set the value of the target position cell to 0 and add it to a deque of tiles that need to be visited.
4) While the deque isn't empty we add the adjacent neighbors of the 1st cell that aren't walls.
5) If we haven't visited that cell yet, we add to the distance it has compared to the target position and set its visited boolean to true.
6) We then add it to the to visit deque.

<img width="285" alt="Screenshot_3" src="https://github.com/Howest-DAE-GD/gpp-researchtopic-Toothles2004/assets/118281295/c75e1456-a0e3-4d39-b8e9-934128ae1de0">

### Generating the flowfield

1) We check all the grid cells, if its distance is 0 we skip and go to the next cell
2) We check all of the cell's neigbors including diagonal
3) We check which one has the closest distance to the target
4) Then we set our cell's velocity vector from the current cell to lowest distance neighbor

### Agents
Now all we need to do is set the velocity of our agents to the vector of the cell they are in and multiply that with their speed.

### Difficulties
When dealing with terrain that has adjacent obstacles but not a diagonal one,
there were some issues where the diagonal path was still the most optimal even though the agent can't pass through.

<img width="153" alt="Screenshot_1" src="https://github.com/Howest-DAE-GD/gpp-researchtopic-Toothles2004/assets/118281295/e67af13d-5a8a-481b-8d74-a066a9edd293">

I fixed this issue by adding an additional check to see if the agents could actually move there.

## Conclusion
Flowfield pathfinding is a powerful technique for efficient navigation in dynamic environments. 
Its benefits in terms of computational efficiency and adaptability make it a valuable tool for game developers. 
Particularly in scenarios involving large crowds or complex terrain.

## Sources
https://www.youtube.com/watch?v=zr6ObNVgytk

http://www.gameaipro.com/GameAIPro/GameAIPro_Chapter23_Crowd_Pathfinding_and_Steering_Using_Flow_Field_Tiles.pdf

https://code.tutsplus.com/understanding-goal-based-vector-field-pathfinding--gamedev-9007t

https://leifnode.com/2013/12/flow-field-pathfinding/

https://howtorts.github.io/2014/01/04/basic-flow-fields.html

https://howtorts.github.io/2013/12/31/generating-a-path-dijkstra.html
