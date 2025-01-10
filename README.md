# FAST

This project contains two folders for conducting experiments using the proposed FastScheduler (FAST) algorithm to schedule traffic flows.

## Data Configuration

### Input

The data configuration is stored in the following three files:

#### 1. `1device.txt`

- **First line**: Records network parameter information in eight parts:
  1. Time granularity (`timeScope`)  
  2. Time synchronization accuracy (`timeAccuracy`)  
  3. Time slot (`timeSlot`)  
  4. Maximum frame length (`maxFrame`)  
  5. Maximum line length (`maxLine`)  
  6. Network line rate (`lineRate`)  
  7. Number of devices (`devnumber`)  
  8. Number of links (`linknumber`)  

  **Example**:
1 1000 20000 1518 100 1000 44 94


- **Second line to the next blank line**: Records switch parameters in six parts:
1. Device ID (`devid`)  
2. Receiving delay (`rdelay`)  
3. Forwarding delay (`fdelay`)  
4. Processing delay (`pdelay`)  
5. Store-and-forward delay (`sfdelay`)  
6. `isterminal` (whether the device is a switch or a terminal)  

**Example**:
0 15000 0 800 15800 1 1 15000 0 800 15800 1


- **From the blank line to the end**: Records data link information in four parts:
1. Link ID (`linkid`)  
2. Preceding node ID (`predev`)  
3. Succeeding node ID (`postdev`)  
4. Link delay (`ldelay`)  

**Example**:
1 3 230 40 2 230 3 40


#### 2. `2flow.txt`

- **First line**: Records the number of traffic flows to be scheduled.  
- **From the second line**: Records information for each traffic flow in seven or eight parts:
1. Flow ID (`fid`)  
2. Frame length (`length`)  
3. Flow period (`period`)  
4. End-to-end delay (`etedelay`)  
5. Number of receiving terminals (`rdevnumber`)  
6. Number of links in the route (`linknumber`)  
7. Number of schedulable links in the route (`schedlinknumber`)  
8. (Optional) Time slot for sending the flow  

**Example**:
1 803 32000000 32000000 1 4 0 2 2 556 4000000 4000000 1 5 0 3


#### 3. `3flowlink.txt`

- Each line records the route of a traffic flow, represented as a set of directed edges, with the sending terminal listed first.  
**Example**:
231 3 3 2 2 1 1 202 228 3 3 2 2 6 6 9 9 215

### Output

The FAST algorithm generates results in six files:

1. **`breakloop.txt`**  
 - Stores the flow ID (`fid`) and data link node information for each flow pushed into the stack during loop breaking.

2. **`urgency.txt`**  
 - Stores the IDs of unschedulable flows (`fid`) and the corresponding data link nodes.

3. **`print.txt`**  
 - Stores the state information of each traffic flow at every data link node.

4. **`offsetresult.txt`**  
 - Stores the `offset` for each flow at every device.  
   - The final schedule is calculated by multiplying `offset` by the time slot (20000ns).  
   - If `offset` is `2147483647`, the flow is unschedulable.

5. **`linkresult.txt`**  
 - Stores the `offset` of flows passing through each data link node.

6. **`stack.txt`**  
 - Records the number of broken loops, scheduling time, and the count of unschedulable flows.

## Execution

- Modify the number of flows to be scheduled in the first line of `2flow.txt`.  
- The scheduling results will be output to the six files listed above.

Thank you for reading!




