import { useEffect } from 'react';
import { LinearProgress } from "@mui/material";
import { useState } from "react";
import JSZip from "jszip";
import styles from './Downloader.module.css';

type DownloaderProps = {
    files: string[]
};

export const Downloader = ({ files }: DownloaderProps) => {

    const [progress, setProgress] = useState(0);

    useEffect(() => {
        if(!files || files.length == 0) {
            return;
        }

        const zipper = new JSZip();

        const promises = files.map(file =>
            fetch(`/photos/${file}`)
                .then(data =>
                    data.arrayBuffer().then(buffer => new Promise(resolve => {
                        zipper.file(file, buffer);
                        setProgress(value => value + 1);
                        resolve(0);
        }))));

        Promise.all(promises).then(() => {
            zipper.generateAsync({type : "uint8array"}).then(blob => {
                const downloader = window.document.createElement('a');
                downloader.href = window.URL.createObjectURL(new Blob([blob]));
                downloader.download = "photos.zip";
                downloader.style.display = 'none';
                (document.body || document.documentElement).appendChild(downloader);
                if(typeof downloader.click === 'function') {
                    downloader.click();
                } else {
                    downloader.target = '_blank';
                    downloader.dispatchEvent(new MouseEvent('click', {
                        view: window,
                        bubbles: true,
                        cancelable: true
                    }));
                }
                URL.revokeObjectURL(downloader.href);
            });
        });
    }, [files, setProgress]);

    return (
        <>
            <LinearProgress className={styles.downloadProgress} variant="determinate" value={progress * 100 / files.length} />
        </>
    );
}