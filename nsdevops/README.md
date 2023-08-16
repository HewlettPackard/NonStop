# NSDevOps Starter Kits 
The starter kits  demonstrate DevOpsfor various language profiles supported by HPE NonStop.
They provide Continuous Integration (CI) & Continuous Deployment (CD)  DevOps pipelines that can be used to demonstrate the DevOps tool chains for the various languages supported by NonStop. 
The pipelines can used as a reference for your own project.
The starter kits also provide instructions to for usage with cloud based DevOps Services such as AzureDevOps and AWS CodeBuild 

## Contents 

The following are the starter kits available 
| Name                  | Description                                                                    | Folder  |
|-----------------------|--------------------------------------------------------------------------------|---------|
| Java Starter Kit      | CI sample for Java based applications built off-platform                       | java    |
| Java JNI Starter Kit  | CI sample for polygot Java and C applications built on-platform on NonStop     | javajni |
| Pyton Starter Kit     | CI sample for Pythons based applications                                       | python  |
| C Starter Kit         | CI sample for C/C++ based OSS applications built off-platform                  | c       |
| CD Starter Kit        | Continuous Deployment using NonStop Manageability Framework (NSMF) and Ansible | cd      |

Each starter kit comprises of 
1. A sample application (typically language specific client-server application)
2. A set of pipeline scripts 
3. A README file the describes the steps to setup the DevOps Tools chain, pipelines and how to use the start kit 

## System Requirements 
Most of the samples of in the NSDevOps Starter Kits is based on the Centralized Jenkins Setup. 
![image](https://media.github.hpe.com/user/32719/files/2d1aa43a-b144-4d13-b7dd-137ad0d6f213)


Hence the following are the system requirement 
1.Developer PC - A Windows/Linux PC that is the developer PC hosting the development tools such as NSDEE and GIT
2.Jenkins Master Node - A Windos/Linux Machine that hosts the Jenkins, SCM, SAST & DAST tools, Artifact Repository and NonStop cross-compilers
3.NonStop Node - A NonStop Machine that is mapped as node in Jenkins on which the Jenkins agent will run. 
SSH access is required to the NonStop node from the centralized Jenkins Master Node. 

## Using the Starter Kits 
**Following are the steps to use the starter-kits **
1. Clone to repository to your PC. 
2. Setup the DevOps Environment following the steps given in the README file 
4. Update the Application/Test Suite IP addresses as per your setup 
5. Update the place holders in the pipeline scripts with values in accordance with your setup
6. Upload to the your GIT Repository and trigger the pipeline 

**Following are the steps to re-use the pipeline scripts for your application **
1. Update the pipeline scripts to point to the GIT Repository of your application 
2. Update the scripts to setup the environment (build, test and dependencies) in accordance to your application 
3. Update the scripts copy your application and its testware, and deploy your application on NonStop
4. Upload the updated script to Jenkins 
5. Trigger the pipeline script to build, test and deploy your application 
