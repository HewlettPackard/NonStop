# BankApp
This repository contains ansible playbook and configuration files for creating a run time environment for applications on the Nonstop Server.

bankapp.yml is the playbook which does the following:

- Downloads an application from the Nexus repository, which was built and uploaded by the process of continuos integration
- Delivers an application to the Nonstop Server for a specific role of an user
- Creates a pathway environment on the Nonstop Server as per the role of an user
- Deploys an application in a pathway environment on the Nonstop Server

Following are user roles and environments:

1. DEV
   - dev user creates single serverclass and single server process and verifies the application deployment
   - In this role user uses minimal required configurations parameters for the environment
2. QA
   - qa user creates single serverclass with four server processes and verifies an application deployment
   - In this role user uses standard and widely used configurations parameters for the environment
   
3. PRODUCTION
   - production user creates single serverclass with ten server processes
   - In this role user uses almost all possible configurations parameters for the environment

