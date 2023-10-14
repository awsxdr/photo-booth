import { AppBar, Box, Toolbar, Typography } from "@mui/material";

export const Navigation = () => {
    return (
        <Box sx={{ flexGrow: 1 }}>
            <AppBar position="fixed">
                <Toolbar>
                    <Typography variant='h6' component='div' sx={{ flexGrow: 1 }}>
                        Wedding pics
                    </Typography>
                </Toolbar>
            </AppBar>
        </Box>
    )
};