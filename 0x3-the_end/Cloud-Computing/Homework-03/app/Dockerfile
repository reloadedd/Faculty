FROM node:16

# Create working directory for app
WORKDIR /usr/src/app

COPY package*.json ./
RUN npm install
COPY . .
EXPOSE 1337

CMD ["node", "hello.js"]

