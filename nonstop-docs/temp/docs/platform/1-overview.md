---
sidebar_label: 'Our Secret to Uptime'
sidebar_position: 1
---

# Our Secret to Uptime

NonStop is a platform that runs some of the world’s most exciting workloads in our day-to-day life. From producing luxury cars, to making payments in our grocery shopping, to helping people travel and executing massive amount of transactions in stock exchanges, NonStop is the platform that lets our customers, and their engineers get their sleep, while their mission-critical applications continue relentlessly in data centres and on private clouds.

# TO REWRITE

## The Art - "Many Nodes but One System" Administration

To the user, the highly reliable system spanning up to 1000+ CPUs looks like one system - though they are many, all the nodes / CPUs can be accessed through one window.

There is no worry about version control across the nodes, and any code to handle failures - they are mirrored across the entire system and **the system is seen as one**.

## The Artist - 
The NonStop OS runs applications and its processes based on a shared-nothing architecture, with mirrored paths between instructions etc.

It is [IDC Availability Level 4](https://blog.stratus.com/what-exactly-is-high-availability-anyway/#:~:text=Zero%20End%20User%20Impact%20(No%20Downtime%20%E2%80%93%20IDC%20calls%20this%20AL4)&text=This%20means%20that%20there%20is,the%20event%20of%20a%20failure) and provides continuous, uninterrupted operation in the event of failure.

This means we specialise in applications that require clustering for highest availabilities.

![Simplicity is Best](/img/simplicity.PNG)

## Patented Process Pair Technology

The NonStop OS has patented its fault-tolerant and self-healing technology by allowing paired processes to takeover the roles of a component that failed.

### How does it work when there is failure?

1. Component fails.

2. Mirrored component takes over the role of failed component. 

3. The failed component reboots itself, and sets itself as the mirror.

4. Processes are mirrored once again.

The process can repeat as many times as possible as necessary, and ensures the **highest level of availability** qualified by IDC.


