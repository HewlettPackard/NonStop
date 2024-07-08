---
sidebar_label: 'Database Architecture'
sidebar_position: 2
---

# Database Architecture

SQL/MX works like other SQL databases. One of the differences would be that SQL/MX subcategorises data in the database into 3 layers.
- Catalog
- Schema
- Table

A catalog can have many schemas, and a schema can have many catalogs. Most other SQL databases have only 2 layers - database and tables.

Here is an example of a company that has its data sorted in the SQL/MX database:
![Catalog Schema](/img/catalogschema.PNG)

# IT Administrator's Bandwidth Freed Up

The IT Administrator usually handles the pressure of ensuring key IT systems are up and often spend weekends to do their maintenance work. 

With the NonStop and its converged hardware, the complexity of fault-tolerance is handled by the OS at the kernel level since its creation. IT Administrators can take advantage of its design, to even update software and hardware while the database is online.

In the following segments, this tutorial will guide you to handle CRUD operations using the Java-specific JDBC driver and ODBC driver.
