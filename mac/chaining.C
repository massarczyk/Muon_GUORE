void chaining(void)
{

  TChain *chain = new TChain("fTree", "fTree");
  chain->Add("./output*.root");
  chain->Merge("all.root");
}
