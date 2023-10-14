import { Stack, Typography } from "@mui/material";
import { PageTemplate } from "./PageTemplate";
import { ImageUploadItem, MultiImageUpload } from "../components";
import { useState } from "react";

export const UploadPage = () => {

    const [files, setFiles] = useState<ImageUploadItem[]>([]);
    const [isUploading, setIsUploading] = useState(false);
    const [uploadsComplete, setUploadsComplete] = useState(false);

    const handleUpload = () => {
        setIsUploading(true);

        let completedUploadCount = 0;
        const handleUploadComplete = () => {
            ++completedUploadCount;

            if(completedUploadCount === files.length) {
                setUploadsComplete(true);
            }
        }

        files.forEach(file => {
            const request = new XMLHttpRequest();

            request.upload.addEventListener('loadstart', () => {
                file.uploadProgress = 0;
                setFiles(files);
            });

            request.upload.addEventListener('progress', (event) => {
                file.uploadProgress = event.loaded / event.total * 100;
                setFiles(files);
            });

            request.upload.addEventListener('load', () => {
                file.uploadProgress = 100;
                handleUploadComplete();
            });

            const data = new FormData();
            data.append("image", file.file);

            request.open("POST", "/api/photos", true);
            request.send(data);
        });
    };

    return (
        <PageTemplate selectedTab={1}>
            <Stack spacing={2}>
                { !uploadsComplete && <MultiImageUpload files={files} isUploading={isUploading} onFilesSelected={setFiles} onUploadClicked={handleUpload} /> }
                { uploadsComplete && <Typography variant="h3">Upload complete!</Typography>}
            </Stack>
        </PageTemplate>
    );
};