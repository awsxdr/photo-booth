import { Downloader } from "../components";
import { usePhotosContext } from "../contexts/PhotosContext";
import { PageTemplate } from "./PageTemplate";

export const DownloadPage = () => {
    const { photos } = usePhotosContext();

    return (
        <PageTemplate selectedTab={-1}>
            <Downloader files={photos} />
        </PageTemplate>
    );
}