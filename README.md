
# Features/Capabilities

* cmake macro that can be included by CARMA packages ("carma_project")
    * A single source for compiler settings, etc.
* Lifecycle Management
* Subsystems
    * Lifecycle management
* System Alerts
* Composable Nodes
    * Likely used per-subsystem

* Recovery
    * What to do here?

# Architecture Questions

* Is there a lifecycle manager for each subsystem?
    * With one central lifecycle manager that interacts with the subsystems?
* How does the CARMA architecture intersect with Docker containers?  Per subsystem?
