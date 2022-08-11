import React, { ChangeEvent, FormEvent, useState } from "react";
import { httpClient } from "./http";

export const App: React.FC = () => {
  const [selectedFile, setSelectedFile] = useState<null | File>();

  const onFileChange = (e: ChangeEvent<HTMLInputElement>) => {
    setSelectedFile(e.target.files![0]);
  };

  const createUploadSession = async (fileSize: number): Promise<string> => {
    return (
      await httpClient.post("/video/uploads/create", { file_size: fileSize })
    ).data.token;
  };

  const onSubmit = (e: FormEvent<HTMLFormElement>) => {
    e.preventDefault();

    if (!selectedFile) {
      alert("No file selected");
      return;
    }

    const fileReader = new FileReader();

    fileReader.onload = async (ev) => {
      const CHUNK_SIZE = 1048576; // 1 MB in bytes
      const fileSize = (ev.target!.result as ArrayBuffer)!.byteLength;

      const uploadToken = await createUploadSession(fileSize);

      const chunks =
        (ev.target!.result as ArrayBuffer)!.byteLength / CHUNK_SIZE;

      let chunksSent = 0;

      for (let i = 0; i < chunks + 1; i++) {
        const chunk = (ev.target!.result as ArrayBuffer)!.slice(
          i * CHUNK_SIZE,
          i * CHUNK_SIZE + CHUNK_SIZE
        );

        if (chunk.byteLength === 0) {
          break;
        }

        await httpClient.post("/video/upload", chunk, {
          headers: {
            "Content-Type": "application/octet-stream",
            "x-utoken": uploadToken,
            "Content-Range": `${chunksSent + chunk.byteLength}`,
          },
        });
        // Sending content-range header to let server know how many bytes it has already received of the full file
        chunksSent += chunk.byteLength;
      }

      alert("Finished uploading");
    };

    fileReader.readAsArrayBuffer(selectedFile);
  };

  return (
    <div className="App">
      <h1>Video uploader</h1>
      <form onSubmit={onSubmit}>
        <input type="file" onChange={onFileChange} />
        <button type="submit">Upload</button>
      </form>
    </div>
  );
};
