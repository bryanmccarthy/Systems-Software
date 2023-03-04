C19749651
Systems Software Assignment 1

Description:
The CTO has offered a list of desired functionality for the new website management model:
1. The company will offer a new shared directory for the 4 departments to upload their xml reports. Authorised users can make changes to directory. The reports will be collected on a nightly basis at 1am and moved to the directory for the dashboard system for processing.
2. The dashboard directory content should be backed up every night.
3. The changes made to the department managers upload directory needs to documented. The username of the user, the file they modified and the timestamp should be recorded.
4. No changes should be allowed to be made to the directories (upload and reporting) while the backup/transfer is happening.
5. If a change needs to be urgently made to the live site, it should be possible to make the changes. (No users should have write access to the new reporting directory)

TODO:
[] Create a daemon to continually manage the operation of the requirements listed by the CTO above.

[] Identify new or modified xml reports and log details of who made the changes, this should be generated as a text file report and stored on the server.

[] The department managers must upload their xml reports by 11:30pm each night. The uploads to the shared directory must be moved to the reporting directory.

[] If a file wasn't uploaded this should be logged by the system.

[] When the backup/transfer begins no user should be able to modify either the upload or reporting directory.

[] It must be possible to ask the daemon to backup and transfer at any time.

[] IPC should be set up to allow all processes to report in on completion of a task. (success or failure)

[] Error logging and reporting should be included for all main tasks in the solution.

[] Create a Makefile to manage the creation of the executables.
