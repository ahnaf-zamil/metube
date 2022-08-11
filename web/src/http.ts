import axios from "axios";

export const httpClient = axios.create({
  withCredentials: true,
  baseURL: "//localhost:5000",
});
