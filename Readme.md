# IMAP client implementation for computer network course in Vilnius university (2022, spring semester).

## Implemented IMAP commands:
- Send and receive imap message
- Check connection status
- Login user
- Login user hardcoded
- Logout user
- Select mailbox by name provided
- Select mailbox by name
- Get mailboxes
- Create mailbox
- Delete mailbox
- Rename mailbox
- Noop
- Get email count for mailbox
- Delete email from mailbox
- Move email from one mailbox to another
- Get mail by uid
    - and receive attachement if it is in the email(Displays txt file in terminal, other types just saves in the project folder).
- Search
- Get all emails from mailbox

## Running the program
```bash
make init
vim .env
#enter your email credentials.
```
```bash
make build
```

## Resources

For the implementation of IMAP client I used the following sources:
- https://datatracker.ietf.org/doc/html/rfc3501

---
## 2024 update
Since google does not allow anymore the basic authentication https://workspaceupdates.googleblog.com/2023/09/winding-down-google-sync-and-less-secure-apps-support.html, oauth method needs to be implemented.