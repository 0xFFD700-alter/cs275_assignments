[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/xy7CGBrk)
**Assignment 2 - Cloth Simulation**
===================================

Introduction
------------

In assignment 2, you are expected to use C++ codes with OpenGL to implement a simple mass spring solver to realize a cloth simulation from scratch. For this assignment, we will provide some reference codes to show you a simple OpenGL framework with an example rectangular piece of cloth. You can either choose to use or not to use these reference resources and modify whatever you want. It is all up to your decision.

General Guidance
----------------

1.  **Plagiarism is always strictly forbidden.**
2.  These assignments are designed to help you learn the algorithms and meanwhile practice your programming skills. **We may or may not provide reference resources and everything is up to your design.**
3.  Various implementations are accepted as long as the results are satisfactory. You can use your favorite programming language or frameworks if it is not specified in assignment.
4.  **It is always requested to explain your codes to our TA after submission.**
5.  It is encouraged to utilize open source projects or third party libraries for **non-core** part (tagged with **\[non-core\]**) of your homework. **BUT you are requested to also explain the key part of how their codes work**. Usage of misunderstood or mysterious codes leads to a score deduction.
6.  You need to implement by yourself for the **core** part (tagged with **\[core\]**).
7.  Feel free to send emails to TA or post questions on Piazza for any help.
8.  Good luck and have fun!

Reading Materials
-----------------

The implementation would become easier if you understand the whole process before you start. There are some materials which might be helpful. Have a quick look before you start.

1.  You can play with a toy demo of a cloth simulation with mass spring [here](https://jklintan.github.io/Cloth-Simulation/).
2.  You can play with another toy demo of a mass spring [here](https://www.physicsclassroom.com/Physics-Interactives/Waves-and-Sound/Mass-on-a-Spring/Mass-on-a-Spring-Interactive) if you feel confused with stiffness and damping.
3.  [Here](https://graphics.stanford.edu/~mdfisher/cloth.html), [here](https://www.ics.uci.edu/~shz/courses/cs114/docs/proj3/index.html) and [here](https://steven.codes/blog/cloth-simulation/) provide some good references about the cloth simulation with the mass spring system.
4.  And always, you can refer to our course [slides](https://faculty.sist.shanghaitech.edu.cn/faculty/liuxp/course/cs275/) about the cloth simulation.

Requirements
------------

1.  **\[core\]** Initialize particles of the mass spring system
    1.  Input: parameters for a piece of rectangular cloth
    2.  Hint: you should decide which physical properties of the particles should be saved.
    3.  Hint: the mass of the particles should sum up to the mass of the cloth.
2.  **\[core\]** Design the spring to connect two particles.
    1.  Hint: consider the data you need for the spring and its force.
3.  **\[core\]** Build the Mass Spring System to form the cloth
    1.  Input: particles you build in step 1.
    2.  Algorithm: connect particles with strings in the system.
    3.  Hint: refer to the paper [here](http://kucg.korea.ac.kr/education/2005/CSCE352/paper/provot95.pdf) about the mass spring model to simulate the cloth.
4.  **\[core\]** Implement the solver for the Mass Spring System
    1.  Input: states of all particles and springs
    2.  Algorithm: compute the forces at all particles and integrate them over time to update the positions of the particles.
    3.  Hint: either a semi-implicit Euler integrator or a velocity-verlet integrator works good enough.
5.  **\[core\]** Add constraint to the position of the particle
    1.  Hint: you can either fix the position after updating or give the particle a large mass.
6.  **\[core\]** Add damping to the mass spring system.
    1.  Hint: the particle forces should be damped proportional to its velocity
7.  **\[core\]** Build your simulator of the whole pipeline with the mass spring system to simulate the cloth.
8.  **\[non-core\]** Visualization system
9.  **\[optional\]** Implement an implicit solver
    1.  You can choose to implement an implicit nonlinear solver by solving an optimization problem as referred to the paper [here](http://tiantianliu.cn/papers/liu13fast/liu13fast.html).

Checkpoints
-----------

1.  **\[Req. 7\]** Visualize the cloth without any dynamics. _\[5 pts\]_
    
2.  **\[Req. 1, 2, 3, 4\]** Test your mass spring solver without gravity by adjusting the resting distance a little smaller/larger than initialization. _\[30 pts\]_
    
3.  **\[Req. 5, 6\]** Simulate the behavior of a piece of rectangular cloth under the gravity where left up and right up corner is fixed. _\[50 pts\]_
    
4.  In the simulation, the cloth is stopped and remains stable finally. _\[15 pts\]_
    
5.  **\[optional\]** **\[Req. 9\]** Implement the implicit solver by referring to the paper [here](http://tiantianliu.cn/papers/liu13fast/liu13fast.html). _\[30 pts\]_
    

Reference Resources and Submission
----------------------------------

This semester, we take advantage of the github classroom to send and collect assignments. You can access [this link](https://classroom.github.com/a/xy7CGBrk) to accept the repository with the reference codes and example models we provide. **Please clone the repository down** to write your homework and **submit with `git commit` and `git push`.** If you are not familiar with git or github, please take a look at [the github document](https://docs.github.com/en/get-started/using-git/about-git).

If you decide to start from scratch and use little reference resources, you can put your work in the `submission` folder. Otherwise, you can modify and add codes in the `reference` folder and commit directly.

Submission Deadline: <span style="color:red">**22:00 on April 21th 2023**</span>

Late hand-in policy
-------------------

Each student is allocated a total of <span style="color:green">**five**</span> late-day points for the whole semester, which work as follows:

*   A student can extend a programming assignment deadline by one day using one point.
*   If a student does not have remaining late-day points, late hand-ins will lead to a deduction of 10% of the total score per day.
*   No assignment will be accepted more than five days after the deadline. This is true whether or not the student has late-day points remaining.
*   We will strictly follow the rule above for late-hand-in policy unless you have a **VERY STRONG** reason, which should be explained to the course instructor and TAs.

* * *

ShanghaiTech CS275 2023 Spring