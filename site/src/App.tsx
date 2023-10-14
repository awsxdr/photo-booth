import { ThemeProvider } from '@emotion/react';
import './App.css'
import { Routes } from './Routes';
import { PhotosContextProvider } from './contexts/PhotosContext'
import { CssBaseline, createTheme } from '@mui/material';

const theme = createTheme({
  palette: {
    mode: 'dark',
    primary: {
      light: '#ffddff',
      main: '#646',
      dark: '#222',
      contrastText: '#ddd'
    }
  }
});

export const App = () => {

  return (
    <>
      <ThemeProvider theme={theme}>
        <CssBaseline />
        <PhotosContextProvider>
          <Routes />
        </PhotosContextProvider>
      </ThemeProvider>
    </>
  )
};