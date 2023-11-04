import { RouterProvider, createBrowserRouter } from "react-router-dom";
import { DownloadPage, GalleryPage, SplashPage, UploadPage } from './pages';

const router = createBrowserRouter([
    {
        path: '/',
        element: <SplashPage />
    },
    {
        path: '/gallery',
        element: <GalleryPage />
    },
    {
        path: '/upload',
        element: <UploadPage />
    },
    {
        path: '/download',
        element: <DownloadPage />
    }
]);

export const Routes = () => (
    <RouterProvider router={router} />
);
