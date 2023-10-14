import { Avatar, Button, Stack } from "@mui/material";
import { PageTemplate } from "./PageTemplate";

export const SplashPage = () => {

    return (
        <PageTemplate selectedTab={-1}>
            <Stack spacing={2} useFlexGap>
                <Avatar alt="Tim and Leanne smiling" src="/tl.jpg" sx={{ width: 196, height: 196, alignSelf: 'center', mb: 4 }} />
                <Button variant="contained" href="/upload">Upload photo</Button>
                <Button variant="contained" href="/gallery">View photos</Button>
            </Stack>
        </PageTemplate>
    );
}