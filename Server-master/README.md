# Capstone Project Information - RESTful API
## Name:
- Design and construction automatica train barrier
## Vietnamese:
- Thiết kế và thi công hệ thống tự động nâng rào chắn khi có tàu
## Supervisor:
- (Mr.) Nguyễn Đức Lợi - loind@fpt.edu.vn
## Team Members
- Hoang Phi Long

## Summary
This repository is a RESTful API part of the capstone projects.
<br />API includes:

* User API - /api/users/(:userId)
* Authenticates API - /api/auth/login/(product)
* Products API - /api/products/(:productId)
* Models API - /api/models/(:modelId)
* Actions API - /api/actions/(:actionId)

### Server
- NodeJS
- ExpressJS
- MongoDB

## Installation
### Requirement
- NodeJS 10+
- Yarn Package Manager 1.7.0
- Git-scm

### Building server
1. Cloning repository
```sh
git clone https://github.com/longhoang0304/DCDCS_Server.git
```
2. Change directory to DCDCS folder
```sh
cd DCDCS_Server
```
3. Install packages and dependencies
```sh
yarn install
```
4. Start the server
```sh
yarn start
```
