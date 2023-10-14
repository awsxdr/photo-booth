import { AddPhotoAlternate, CloudUpload } from "@mui/icons-material";
import { LoadingButton } from "@mui/lab";
import { Button, ImageList, ImageListItem, LinearProgress, Paper, Stack, styled } from "@mui/material";
import { useEffect, useRef, useState } from "react";

interface MultiImageUploadProps {
    files: ImageUploadItem[];
    isUploading?: boolean;
    onFilesSelected: (files: ImageUploadItem[]) => void;
    onUploadClicked: () => void;
};

export type ImageUploadItem = {
    file: File;
    uploadProgress: number;
};

const HiddenInput = styled('input')({
    clip: 'rect(0 0 0 0)',
    clipPath: 'inset(50%)',
    height: 1,
    overflow: 'hidden',
    position: 'absolute',
    bottom: 0,
    left: 0,
    whiteSpace: 'nowrap',
    width: 1,
});

const readFile = (file: Blob) => new Promise<string>((resolve, reject) => {
    const reader = new FileReader();

    reader.addEventListener('load', () => {
        resolve(reader.result?.toString() ?? "");
    });
    reader.addEventListener('error', reject);

    reader.readAsDataURL(file);
});

export const MultiImageUpload = ({ files, isUploading, onFilesSelected, onUploadClicked }: MultiImageUploadProps) => {

    const uploadInputRef = useRef<HTMLInputElement>();

    useEffect(() => {
        uploadInputRef.current = document.getElementById('imageInput')! as HTMLInputElement;
    }, []);

    const [images, setImages] = useState<string[]>([]);

    const handleFileAdded = () => {

        const imageFiles = 
            Array.from(uploadInputRef.current?.files ?? [])
                .filter(file => file.type.startsWith('image/'));
        
        onFilesSelected(imageFiles.map(f => ({ file: f, uploadProgress: 0 })));
    }

    Promise
        .all(files.map(f => readFile(f.file)))
        .then(setImages);

    return (
        <>
            <Stack>
                <Button component="label" variant="contained" startIcon={<AddPhotoAlternate />} disabled={isUploading}>
                    Pick photos
                    <HiddenInput type="file" accept="image/*" id="imageInput" onChange={handleFileAdded} multiple />
                </Button>
                <ImageList>
                    {
                        images
                            .map((image, index) => ({ data: image, uploadProgress: files[index].uploadProgress }))
                            .map(image => (
                            <ImageListItem>
                                <img src={image.data} />
                                { isUploading && <LinearProgress variant="determinate" value={image.uploadProgress} /> }
                            </ImageListItem>
                        ))
                    }
                </ImageList>
            </Stack>
            <Paper sx={{ position: 'fixed', bottom: 56, left: 0, right: 0 }} elevation={3}>
                <LoadingButton fullWidth variant="contained" loading={isUploading} disabled={isUploading || files.length == 0} startIcon={<CloudUpload />} onClick={onUploadClicked}>
                    Upload
                </LoadingButton>
            </Paper>
        </>
    );
};