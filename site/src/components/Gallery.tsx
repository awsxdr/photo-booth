import { ImageList, ImageListItem } from "@mui/material";
import './Gallery.css';
import { useEffect, useState } from "react";

interface GalleryProps {
    photoNames: string[],
};

export const Gallery = ({ photoNames }: GalleryProps) => {

    const [photosShown, setPhotosShown] = useState(6);

    useEffect(() => {
        window.addEventListener('scroll', () => {
            if(photosShown < photoNames.length && window.scrollY + window.innerHeight * 2 > document.body.scrollHeight) {
                setPhotosShown(value => value + 4);
            }
        });
    }, [photoNames, setPhotosShown]);

    return (
        <ImageList>
            {
                photoNames.slice(0, photosShown).map(photo => (
                    <ImageListItem key={photo}>
                        <img src={`/photos/thumbnails/${photo}`} className="galleryImage" onClick={() => document.location.href = `/photos/${photo}`} />
                    </ImageListItem>
                ))
            }
        </ImageList>
    )
}